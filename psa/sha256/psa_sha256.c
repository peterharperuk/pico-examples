/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
// Include sys/types.h before inttypes.h to work around issue with
// certain versions of GCC and newlib which causes omission of PRIu64
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "psa/crypto.h"

#define BUFFER_SIZE 10000

int main() {
    stdio_init_all();

    // Initialise the PSA crypto library
    psa_status_t status = psa_crypto_init();
    hard_assert(status == PSA_SUCCESS);

    // nist 3
    uint8_t *buffer = malloc(BUFFER_SIZE);
    memset(buffer, 0x61, BUFFER_SIZE);
    const uint8_t nist_3_expected[] = { \
        0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92, 0x81, 0xa1, \
        0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67, 0xf1, 0x80, 0x9a, 0x48, \
        0xa4, 0x97, 0x20, 0x0e, 0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, \
        0x2c, 0xd0 };

    // check PSA hw accelerated speed
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
    uint64_t start = time_us_64();
    status = psa_hash_setup(&op, PSA_ALG_SHA_256);
    hard_assert(status == PSA_SUCCESS);
    for (int i = 0; i < 1000000; i += BUFFER_SIZE) {
        status = psa_hash_update(&op, buffer, BUFFER_SIZE);
        hard_assert(status == PSA_SUCCESS);
    }
    unsigned char psa_result[32];
    size_t hash_length = 0;
    status = psa_hash_finish(&op, psa_result, sizeof(psa_result), &hash_length);
    hard_assert(status == PSA_SUCCESS);
    uint64_t psa_time = (time_us_64() - start) / 1000;
    printf("PSA time for sha256 of 1M bytes %" PRIu64 "ms\n", psa_time);
    hard_assert(hash_length == 32);
    hard_assert(memcmp(nist_3_expected, psa_result, 32) == 0);
    psa_hash_abort(&op);
    //hard_assert(psa_time < 50); // less than 50ms
    free(buffer);
    printf("Test passed\n");
    return 0;
}