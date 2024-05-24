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

// This example needs an external clock fed into the GP20
// Note: Only GP20 and GP22 can be used for clock input, See the GPIO function table in the RP2040 datasheet.
// You can use another Pico to generate this. See the clocks/hello_gpout example for more details.
// clock_gpio_init(21, CLOCKS_CLK_GPOUT3_CTRL_AUXSRC_VALUE_CLK_RTC, 1); // 46875Hz can only export a clock on gpios 21,23,24,25 and only 21 is exposed by Pico
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
    while(true) {
        printf("Awake for 10s\n");
        sleep_ms(10000);

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
    }
    return 0;
}