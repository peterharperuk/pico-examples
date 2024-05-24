/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"

#define WAKE_GPIO 10

int main() {
    stdio_init_all();
    printf("Hello Dormant!\n");

    while(true) {
        printf("Switching to XOSC\n");
        uart_default_tx_wait_blocking();

        /*Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
        This is necessary before sending the pico to sleep*/
        sleep_run_from_xosc();

        printf("Running from XOSC\n");
        uart_default_tx_wait_blocking();

        printf("XOSC going dormant until GPIO %d goes high\n", WAKE_GPIO);
        uart_default_tx_wait_blocking();

        // Go to sleep until we see a high edge on GPIO 10
        sleep_goto_dormant_until_edge_high(WAKE_GPIO);

        //Re-enabling clock sources and generators.
        sleep_power_up();
        printf("Now awake\n");
        sleep_ms(1000 * 10);
    }

    return 0;
}