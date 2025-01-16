/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "mbedtls/debug.h"

#include "example_http_client_util.h"

#define HOST "fw-download-alias1.raspberrypi.com"
#define URL_REQUEST "/net_install/boot.sig"

static absolute_time_t timestamp;
static int keep_alive_count;
static EXAMPLE_HTTP_REQUEST_T req;

// Print headers to stdout
static err_t header_print_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, __unused u32_t content_len) {
    printf("\nTime taken for headers %ums\n", (uint32_t)absolute_time_diff_us(timestamp, get_absolute_time()) / 1000);
    return http_client_header_print_fn(connection, arg, hdr, hdr_len, content_len);
}

// Print body to stdout
static err_t receive_print_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err) {
    printf("\nTime taken for body %ums\n", (uint32_t)absolute_time_diff_us(timestamp, get_absolute_time()) / 1000);
    return http_client_receive_print_fn(arg, conn, p, err);
}

// Make the next request
static void request_worker_fn(async_context_t *context, async_at_time_worker_t *worker) {
    timestamp = get_absolute_time();
    httpc_get_next(URL_REQUEST, &req.settings, req.connection);
}
static async_at_time_worker_t request_worker;

static int result_fn(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    if (keep_alive_count > 0) {
        // repeat the request
        keep_alive_count--;

        // Make another request in 1s
        request_worker.do_work = request_worker_fn;
        async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &request_worker, 1000);
        return 1;
    }
    return 0;
}

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect\n");
        return 1;
    }

#ifndef NDEBUG
    // build for debug (slows down TLS handshakes)
    // remove #undef LWIP_DEBUG
    // Turn ON ALTCP_MBEDTLS_DEBUG and ALTCP_MBEDTLS_LIB_DEBUG
    // enable mbedtls debug below, set to 3 to get details
    mbedtls_debug_set_threshold(1);
#endif

    req.hostname = HOST;
    req.url = URL_REQUEST;
    req.headers_fn = header_print_fn;
    req.recv_fn = receive_print_fn;
    req.result_fn = result_fn;

    // Make an insecure http request
    timestamp = get_absolute_time();
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    // Make a secure http request, first request will be slow, second will be quickers as it reuses the same connection
    keep_alive_count = 1;
    timestamp = get_absolute_time();
    req.tls_config = altcp_tls_create_config_client(NULL, 0); // switch to https
    result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (req.tls_config) {
        altcp_tls_free_config(req.tls_config);
    }

    if (result != 0) {
        panic("test failed");
    }
    cyw43_arch_deinit();
    printf("Test passed\n");
    sleep_ms(100);
    return 0;
}