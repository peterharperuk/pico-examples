/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "dnsserver.h"

void run_dns_server(void) {

    ip4_addr_t gw;
    IP4_ADDR(&gw, 192, 168, 4, 1);

    dns_server_t dns_server;
    dns_server_init(&dns_server, &gw);

    while(true) {
        // the following #ifdef is only here so this same example can be used in multiple modes;
        // you do not need it in your code
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer) to check for WiFi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        sleep_ms(1);
#else
        // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(1000);
#endif
    }
    dns_server_deinit(&dns_server);
}

// Just runs the dns server for testing
// e.g. from another PC you can run the following
//  nslookup www.google.com <ip address>
int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("failed to connect\n");
        return 1;
    }
    run_dns_server();
    cyw43_arch_deinit();
    return 0;
}
