/*
 * Copyright (c) 2021 Valentin Milea <valentin.milea@gmail.com>
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/multicore.h"
#include "pio_i2c.h"

static const uint I2C_SLAVE_ADDRESS = 0x17;
static const uint I2C_BAUDRATE = 100000; // 100 kHz

#ifdef i2c_default
// For this example, we run both the master and slave from the same board.
// You'll need to wire pin GP4 to GP6 (SDA), and pin GP5 to GP7 (SCL).
static const uint I2C_SLAVE_SDA_PIN = PICO_DEFAULT_I2C_SDA_PIN; // 4
static const uint I2C_SLAVE_SCL_PIN = PICO_DEFAULT_I2C_SCL_PIN; // 5
static const uint I2C_MASTER_SDA_PIN = 6;
static const uint I2C_MASTER_SCL_PIN = 7;

// The slave implements a 256 byte memory. To write a series of bytes, the master first
// writes the memory address, followed by the data. The address is automatically incremented
// for each byte transferred, looping back to 0 upon reaching the end. Reading is done
// sequentially from the current memory address.
static struct
{
    uint8_t mem[256];
    uint8_t mem_address;
    bool mem_address_written;
} context;

static bool done_slave_init;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!context.mem_address_written) {
            // writes always start with the memory address
            context.mem_address = i2c_read_byte_raw(i2c);
            context.mem_address_written = true;
        } else {
            // save into memory
            context.mem[context.mem_address] = i2c_read_byte_raw(i2c);
            context.mem_address++;
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data

        //assert(false);
        if (!done_slave_init) {
            const char start[] = "START";
            i2c_write_raw_blocking(i2c, (uint8_t*)start, 5);
            done_slave_init = true;
            break;
        }
        // load from memory
        i2c_write_byte_raw(i2c, context.mem[context.mem_address]);
        context.mem_address++;
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        context.mem_address_written = false;
        break;
    default:
        break;
    }
}

static void setup_slave() {
    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SDA_PIN);

    gpio_init(I2C_SLAVE_SCL_PIN);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    gpio_init(8);
    gpio_set_dir(8, GPIO_OUT);
    gpio_put(8, 1);
    sleep_ms(1);
    gpio_put(8, 0);

    i2c_init(i2c0, I2C_BAUDRATE);

    gpio_put(8, 1);
    sleep_ms(1);
    gpio_put(8, 0);

    // configure I2C0 for slave mode
    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);

    gpio_put(8, 1);
    sleep_ms(1);
    gpio_put(8, 0);

}

static void run_pio_master() {
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);

    // read hello
    char start_buf[5];
    int count;
    /*do {
        count = pio_i2c_read_blocking(pio, sm, I2C_SLAVE_ADDRESS, NULL, 0);
    } while(count < 0);
    assert(false);*/
    do {
        count = pio_i2c_read_blocking(pio, sm, I2C_SLAVE_ADDRESS, (uint8_t*)start_buf, 5);
    } while(count < 0);
    printf("master read done\n");
    assert(count == 0);
    assert(memcmp(start_buf, "START", 5) == 0);
    printf("success\n");
}

static void run_master() {
    gpio_init(I2C_MASTER_SDA_PIN);
    gpio_set_function(I2C_MASTER_SDA_PIN, GPIO_FUNC_I2C);
    // pull-ups are already active on slave side, this is just a fail-safe in case the wiring is faulty
    gpio_pull_up(I2C_MASTER_SDA_PIN);

    gpio_init(I2C_MASTER_SCL_PIN);
    gpio_set_function(I2C_MASTER_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_MASTER_SCL_PIN);

    i2c_init(i2c1, I2C_BAUDRATE);

    // read hello
    char start_buf[5];
    int count;
    do {
        count = i2c_read_blocking_until(i2c1, I2C_SLAVE_ADDRESS, NULL, 0, false, make_timeout_time_ms(100));
    } while(count < 0);
    assert(count == 0);
    do {
        count = i2c_read_blocking_until(i2c1, I2C_SLAVE_ADDRESS, (uint8_t*)start_buf, 5, false, make_timeout_time_ms(100));
    } while(count < 0);
    printf("master read done\n");
    assert(count == 5);
    assert(memcmp(start_buf, "START", 5) == 0);
    printf("success\n");

    /*for (uint8_t mem_address = 0;; mem_address = (mem_address + 32) % 256) {
        char msg[32];
        snprintf(msg, sizeof(msg), "Hello, I2C slave! - 0x%02X", mem_address);
        uint8_t msg_len = strlen(msg);

        uint8_t buf[32];
        buf[0] = mem_address;
        memcpy(buf + 1, msg, msg_len);
        // write message at mem_address
        printf("Write at 0x%02X: '%s'\n", mem_address, msg);
        count = i2c_write_blocking(i2c1, I2C_SLAVE_ADDRESS, buf, 1 + msg_len, false);
        if (count < 0) {
            puts("Couldn't write to slave, please check your wiring!");
            return;
        }
        hard_assert(count == 1 + msg_len);

        // seek to mem_address
        count = i2c_write_blocking(i2c1, I2C_SLAVE_ADDRESS, buf, 1, true);
        hard_assert(count == 1);
        // partial read
        uint8_t split = 5;
        count = i2c_read_blocking(i2c1, I2C_SLAVE_ADDRESS, buf, split, true);
        hard_assert(count == split);
        buf[count] = '\0';
        printf("Read  at 0x%02X: '%s'\n", mem_address, buf);
        hard_assert(memcmp(buf, msg, split) == 0);
        // read the remaining bytes, continuing from last address
        count = i2c_read_blocking(i2c1, I2C_SLAVE_ADDRESS, buf, msg_len - split, false);
        hard_assert(count == msg_len - split);
        buf[count] = '\0';
        printf("Read  at 0x%02X: '%s'\n", mem_address + split, buf);
        hard_assert(memcmp(buf, msg + split, msg_len - split) == 0);

        puts("");
        sleep_ms(2000);
    }*/
}
#endif

void core1_entry() {

    sleep_ms(500);
    setup_slave();

    while (1)
        tight_loop_contents();
}

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c / slave_mem_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return 0;
#else
    puts("\nI2C slave example");

    multicore_launch_core1(core1_entry);

    //run_master();
    run_pio_master();
#endif
}
