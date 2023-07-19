#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/resets.h"
#include "pico/sleep.h"

#define TEMPERATURE_UNITS 'C' //Set to 'F' or 'C' for Fahrenheit or Celsius
#define POLLING_INTERVAL 10

void sleep_callback()
{
    printf("RTC woke us up\n");
    uart_default_tx_wait_blocking();
}

float read_onboard_temperature(const char unit) {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C') {
        return tempC;
    } else if (unit == 'F') {
        return tempC * 9 / 5 + 32;
    }

    return -1.0f;
}

void rtc_sleep()
{
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
            .sec   = POLLING_INTERVAL
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    printf("Sleeping for 10 seconds\n");
    uart_default_tx_wait_blocking();

    //All clocks except for RTC (and USB/USB PLL if USB Serial is used) are turned off
    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}


int main()
{
    stdio_init_all();

    //Initialise the ADC and configure it to take input from the onboard temperature sensor
    adc_init();
    adc_select_input(4);
    adc_set_temp_sensor_enabled(true);

    while(true)
    {
        float temperature = read_onboard_temperature(TEMPERATURE_UNITS);
        printf("Temperature is %.02f %c\n", temperature, TEMPERATURE_UNITS);

        // Wait for the fifo to be drained so we get reliable output
        uart_default_tx_wait_blocking();

        /*Set the crystal oscillator as the dormant clock source, UART will be reconfigured from here
        This is necessary before sending the pico to sleep*/
        sleep_run_from_xosc();

        // Go to sleep until the RTC interrupt is generated after the polling interval
        rtc_sleep();

        //Re-enabling clock sources and generators.
        sleep_power_up();

        printf("ROSC Restarted!\n");
    }

    return 0;

}
