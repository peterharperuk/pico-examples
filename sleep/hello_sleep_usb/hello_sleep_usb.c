/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*In its current state, this is unreliable. The program usually runs for some unpredictable amount of time (anywhere in the range 30-100 loops)
  However, after this the USB device fails to enumerate correctly and terminates the program*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "device/usbd.h"

static bool awake;

static void sleep_callback(void) {
    //Note we cannot print any serial output in this ISR as the USB will not be reinitialised yet
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

    //All clocks except for RTC (and USB/USB PLL if USB Serial is used) are turned off
    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}

int main() {
    
    /*Since the USB clock/PLL are disabled in sleep mode, we cannot print any serial output during sleep
      All printf statements are before/after sleep here, in contrast to the UART example */

    stdio_init_all();

    while(true)
    {
        while(tud_task_event_ready())
        {
            tight_loop_contents(); //Nop until the USB xfer is complete, otherwise we might get strange output
        }

        printf("Switching to XOSC\n");

        sleep_run_from_xosc();

        awake = false;

        rtc_sleep();

        // Make sure we don't wake
        while (!awake) {
            printf("Should be sleeping\n");
        }

        //Re-enabling clock sources and generators
        sleep_power_up();
        clocks_init();

        //Re-enumerate the USB device
        //FIXME: Should this go into the sleep_power_up() function so its abstracted away?
        tud_disconnect();
        while(tud_connected())
        {
            tight_loop_contents();
        }
        tud_connect();
        while(!tud_connected())
        {
            tight_loop_contents();
        }

        sleep_ms(1000);

        printf("USB re-initialised!\n");
    }

    return 0;
}
