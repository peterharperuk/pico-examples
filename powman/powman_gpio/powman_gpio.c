/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/powman.h"
#include "powman_example.h"

#ifndef WAKEUP_GPIO
#define WAKEUP_GPIO 15
#endif

// How long to wait
#define PAUSE_TIME_MS 5000

// Got to sleep and wakeup when a GPIO goes high
// The example will repeatedly wait until GPIO 15 is low then switch off until GPIO 15 goes high
// To test this you can connect the GPIO to ground and then 3V3(OUT)
// The debugger will appear to be unresponsive while the device is off
int main() {
    stdio_init_all();

    // Initialise the example
    powman_example_init(1704067200000);

    // Scratch register survives power down
    printf("Wake up, test run: %u\n", powman_hw->scratch[0]++);

    // Stay awake for a few seconds
    printf("Awake for %dms\n", PAUSE_TIME_MS);
    sleep_ms(PAUSE_TIME_MS);

    // power off
    int rc = powman_example_off_until_gpio_high(WAKEUP_GPIO);
    hard_assert(rc == PICO_OK);

    hard_assert(false); // should never get here!
    return 0;
}
