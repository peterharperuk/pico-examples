/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"

#include "hardware/rtc.h"
#include "hardware/clocks.h"

#define EXTERNAL_CLOCK_INPUT_PIN 20
#define RTC_FREQ_HZ 46875

static void sleep_callback(void) {
    printf("RTC woke us up\n");
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

    //Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    printf("Sleeping for 10 seconds\n");
    uart_default_tx_wait_blocking();

    //Go to sleep for 10 seconds, with RTC running off GP20
    //The external clock is the RTC of another pico being fed to GP20
    sleep_goto_dormant_until(&t_alarm, &sleep_callback, RTC_FREQ_HZ, EXTERNAL_CLOCK_INPUT_PIN);

}

int main() {

    stdio_init_all();
    printf("Hello Dormant GPIO!\n");
            
    printf("Switching to XOSC\n");
    uart_default_tx_wait_blocking();

    /*Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
    This is necessary before sending the pico to sleep*/
    sleep_run_from_xosc();

    printf("Running from XOSC\n");
    uart_default_tx_wait_blocking();

    printf("XOSC going dormant\n");
    uart_default_tx_wait_blocking();

    // Go to sleep until the RTC interrupt is generated after 10 seconds
    rtc_sleep();

    //Re-enabling clock sources and generators.
    sleep_power_up();

    printf("ROSC restarted!\n");

    return 0;
}