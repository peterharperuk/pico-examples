/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/powman.h"
#include "powman_example.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/adc.h"
#include "hardware/structs/usb.h"

// How long to wait
#define AWAKE_TIME_MS 10000
#define SLEEP_TIME_MS 5000

static void disable_usb() {
    usb_hw->phy_direct = USB_USBPHY_DIRECT_TX_PD_BITS | USB_USBPHY_DIRECT_RX_PD_BITS | USB_USBPHY_DIRECT_DM_PULLDN_EN_BITS | USB_USBPHY_DIRECT_DP_PULLDN_EN_BITS;
    usb_hw->phy_direct_override = USB_USBPHY_DIRECT_RX_DM_BITS | USB_USBPHY_DIRECT_RX_DP_BITS | USB_USBPHY_DIRECT_RX_DD_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_BITS;
}

// Got to sleep and wakeup after 5 seconds
// The example will repeatedly wait 5 seconds then switch off for 5 seconds
// The debugger will appear to be unresponsive while the device is off
int main() {

    set_sys_clock_48mhz(); // run everything from pll_usb pll and stop pll_sys

    // Set all pins to input (as far as SIO is concerned)
    gpio_set_dir_all_bits(0);
    for (int i = 2; i < NUM_BANK0_GPIOS; ++i) {
        gpio_set_function(i, GPIO_FUNC_SIO);
        if (i > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
            gpio_disable_pulls(i);
            gpio_set_input_enabled(i, false);
        }
    }

    // Unlock the VREG control interface
    hw_set_bits(&powman_hw->vreg_ctrl, POWMAN_PASSWORD_BITS | POWMAN_VREG_CTRL_UNLOCK_BITS);

    stdio_init_all();

    // Turn off USB PHY and apply pull downs on DP & DM
    disable_usb();

    // Initialise the example
    powman_example_init(1704067200000);

    // Scratch register survives power down
    printf("Wake up, test run: %u\n", powman_hw->scratch[0]++);

    // Stay awake for a few seconds
    printf("Awake for %dms\n", AWAKE_TIME_MS);
    sleep_ms(AWAKE_TIME_MS);

    // power off
    int rc = powman_example_off_for_ms(SLEEP_TIME_MS);
    hard_assert(rc == PICO_OK);

    hard_assert(false); // should never get here!
    return 0;
}
