/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"


int main() {
    stdio_init_all();
    printf("Hello gpout\n");

    // Output clk_sys / 10 to gpio 21, etc...
    // Note: Only these GPIOs can be used for clock output, See the GPIO function table in the RP2040 datasheet.
    clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10); // 12.5MHz
    clock_gpio_init(23, CLOCKS_CLK_GPOUT1_CTRL_AUXSRC_VALUE_CLK_USB, 10); // 4.8MHz
    clock_gpio_init(24, CLOCKS_CLK_GPOUT2_CTRL_AUXSRC_VALUE_CLK_ADC, 10); // 4.8MHz
    clock_gpio_init(25, CLOCKS_CLK_GPOUT3_CTRL_AUXSRC_VALUE_CLK_RTC, 1); // 46875Hz

    return 0;
}
