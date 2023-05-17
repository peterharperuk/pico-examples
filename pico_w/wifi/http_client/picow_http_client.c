/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"

#define HOST "worldtimeapi.org"
#define URL_REQUEST "/api/ip"

extern bool run_http_client_test(async_context_t *context, bool use_https, const uint8_t *cert, size_t cert_len, const char *hostname, const char *url);

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("failed to connect\n");
        return 1;
    }
    bool result1 = run_http_client_test(cyw43_arch_async_context(), false, NULL, 0, HOST, URL_REQUEST); // http
    bool result2 = run_http_client_test(cyw43_arch_async_context(), true, NULL, 0, HOST, URL_REQUEST); // https
    if (!result1 || !result2) {
        panic("test failed");
    }
    cyw43_arch_deinit();
    printf("Test passed\n");
    sleep_ms(100);
    return 0;
}