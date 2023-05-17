/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/async_context.h"
#include "lwip/apps/http_client.h"
#include "lwip/altcp_tls.h"

typedef struct HTTP_STATE {
    bool complete;
    httpc_result_t httpc_result;
    struct altcp_tls_config *tls_config;
    altcp_allocator_t tls_allocator;
    httpc_connection_t settings;
    const char *hostname;
} HTTP_STATE_T;

// Gets the content of the headers
static err_t headers_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
    printf("\nheaders %u\n", hdr_len);

    u16_t offset = 0;
    while (offset < hdr->tot_len && offset < hdr_len) {
        char c = (char)pbuf_get_at(hdr, offset++);
        putchar(c);
    }
    return ERR_OK;
}

// Gets the content of the body
static err_t recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err) {
    printf("\ncontent err %d\n", err);
    u16_t offset = 0;
    while (offset < p->tot_len) {
        char c = (char)pbuf_get_at(p, offset++);
        putchar(c);
    }
    return ERR_OK;
}

static void result_fn(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    HTTP_STATE_T *state = (HTTP_STATE_T*)arg;
    printf("result %d len %u server_response %u err %d\n", httpc_result, rx_content_len, srv_res, err);
    state->complete = true;
    state->httpc_result = httpc_result;
}

// Override altcp_tls_alloc to set sni
static struct altcp_pcb *altcp_tls_alloc_sni(void *arg, u8_t ip_type) {
    HTTP_STATE_T *state = (HTTP_STATE_T*)arg;
    struct altcp_pcb *pcb = altcp_tls_alloc(state->tls_config, ip_type);
    if (!pcb) {
        return NULL;
    }
    mbedtls_ssl_set_hostname(altcp_tls_context(pcb), state->hostname);
    return pcb;
}

// Make a http request
bool run_http_client_test(async_context_t *context, bool use_https, const uint8_t *cert, size_t cert_len, const char *hostname, const char *url) {
    HTTP_STATE_T state = { 0 };
    if (use_https) {
        state.tls_config = altcp_tls_create_config_client(cert, cert_len);
        state.tls_allocator.alloc = altcp_tls_alloc_sni;
        state.tls_allocator.arg = &state;
        state.settings.altcp_allocator = &state.tls_allocator;
    } else {
        // Can't use a cert without https
        if (cert || cert_len > 0) {
            return false;
        }
    }
    state.settings.headers_done_fn = headers_fn; // can be null
    state.settings.result_fn = result_fn;
    state.hostname = hostname;
    state.httpc_result = HTTPC_RESULT_ERR_UNKNOWN; // make sure we see real success
    err_t ret = httpc_get_file_dns(hostname, use_https ? 443 : 80, url, &state.settings, recv_fn, &state, NULL);
    if (ret != ERR_OK) {
        panic("http request failed: %d", ret);
    }
    while(!state.complete) {
        async_context_poll(context);
        async_context_wait_for_work_ms(context, 1000);
    }
    if (use_https) {
        altcp_tls_free_config(state.tls_config);
    }
    return state.httpc_result == HTTPC_RESULT_OK;
}