/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/powman.h"
#include "powman_example.h"

// How long to wait
#define PAUSE_TIME_MS 5000

// Got to sleep and wakeup after 5 seconds
// The example will repeatedly wait 5 seconds then switch off for 5 seconds
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
    int rc = powman_example_off_for_ms(PAUSE_TIME_MS);
    hard_assert(rc == PICO_OK);

    hard_assert(false); // should never get here!
    return 0;
}
