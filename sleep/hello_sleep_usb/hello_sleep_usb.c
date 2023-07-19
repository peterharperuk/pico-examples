/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*Note that the USB connection requires a reliable power source to run this program reliably, otherwise the USB connection will drop
after some number of loops*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "device/usbd.h"
#include "tusb.h"

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
    
    /*Since the USB clock/PLL are disabled in sleep mode, we cannot print any serial output after the run_from_dormant_source function
      All printf statements are before/after rtc_sleep here, in contrast to the UART example */

    stdio_init_all();

    while(true)
    {   
        
        //FIXME: Any better way to do this with tight loop contents?
        //Let USB enumerate
        sleep_ms(1000);

        printf("Switching to XOSC\n");

        while(tud_task_event_ready())
        {
            tight_loop_contents(); //Nop until the USB txfer is complete so we get reliable output
        }

        /*Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
        This is necessary before sending the pico to sleep*/
        sleep_run_from_xosc();

        awake = false;

        // Go to sleep until the RTC interrupt is generated after 10 seconds
        rtc_sleep();

        // Make sure we don't wake
        while (!awake) {
            printf("Should be sleeping\n");
        }

        //Re-enabling clock sources and generators
        sleep_power_up();

        //Re-enumerate the USB device
        //FIXME: Should this go into the sleep_power_up() function so its abstracted away?
        tud_disconnect();
        while(tud_connected())  {
            tight_loop_contents(); //Make sure we actually disconnect before attempting to connect again
        }
        tud_connect();
        while(!tud_connected())
        {
            tight_loop_contents(); //Make sure we are connected before continuing execution
        }

    }

    return 0;
}
