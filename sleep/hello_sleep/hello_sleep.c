/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"

static bool awake;

static void sleep_callback(void) {
    printf("RTC woke us up\n");
    uart_default_tx_wait_blocking();
    awake = true;
}

static void rtc_sleep(void) {
    // Start on Wednesday 5th of July 2023 12:00:00
    datetime_t t = {
            .year  = 2023,
            .month = 07,
            .day   = 05,
            .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
            .hour  = 12,
            .min   = 00,
            .sec   = 00
    };

    // Alarm 10 seconds later
    datetime_t t_alarm = {
            .year  = 2023,
            .month = 07,
            .day   = 05,
            .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
            .hour  = 12,
            .min   = 00,
            .sec   = 10
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    printf("Sleeping for 10 seconds\n");
    uart_default_tx_wait_blocking();

    //All clocks except for RTC (and USB/USB PLL if USB Serial is used) are turned off
    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}

int main() {

    stdio_init_all();

    printf("Hello Sleep!\n");

    do {
        printf("Awake for 10 seconds\n");
        sleep_ms(1000 * 10);

        printf("Switching to XOSC\n");

        // Wait for the fifo to be drained so we get reliable output
        uart_default_tx_wait_blocking();

        // Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
        // This is necessary before sending the pico to sleep
        sleep_run_from_xosc();

        printf("Switched to XOSC\n");

        awake = false;

        // Go to sleep until the RTC interrupt is generated after 10 seconds
        rtc_sleep();

        // Make sure we don't wake
        while (!awake) {
            printf("Should be sleeping\n");
        }

        //Re-enabling clock sources and generators.
        sleep_power_up();
    } while(true);

    return 0;
}
