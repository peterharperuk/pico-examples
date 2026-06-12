/**
 * PSA Cryptography API ECDSA example for Raspberry Pi Pico
 *
 * Demonstrates key pair generation, message signing, and signature
 * verification using ECDSA with the NIST P-256 curve and SHA-256.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
// Include sys/types.h before inttypes.h to work around issue with
// certain versions of GCC and newlib which causes omission of PRIu64
#include <sys/types.h>
#include <inttypes.h>

#include "pico/stdlib.h"
#include "psa/crypto.h"

static void print_hex(const char *label, const uint8_t *buf, size_t len) {
    printf("%s (%zu bytes):\n  ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", buf[i]);
        if ((i + 1) % 32 == 0 && i + 1 < len) {
            printf("\n  ");
        }
    }
    printf("\n");
}

int main() {
    stdio_init_all();

    psa_status_t status;
    psa_key_id_t key_id;

    /* ------------------------------------------------------------------ */
    /* Initialise the PSA crypto library                                   */
    /* ------------------------------------------------------------------ */
    status = psa_crypto_init();
    hard_assert(status == PSA_SUCCESS);
    printf("PSA crypto initialised\n\n");

    /* ------------------------------------------------------------------ */
    /* Generate an ECDSA P-256 key pair                                    */
    /* ------------------------------------------------------------------ */
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_SIGN_MESSAGE  |
                            PSA_KEY_USAGE_VERIFY_MESSAGE |
                            PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes,
                     PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);

    uint64_t start = time_us_64();
    status = psa_generate_key(&attributes, &key_id);
    hard_assert(status == PSA_SUCCESS);
    uint64_t keygen_time = (time_us_64() - start) / 1000;
    printf("Key generation time: %" PRIu64 "ms\n", keygen_time);

    /* Export and display the public key (uncompressed: 0x04 || x || y) */
    uint8_t public_key[65];
    size_t public_key_length = 0;
    status = psa_export_public_key(key_id, public_key,
                                   sizeof(public_key), &public_key_length);
    hard_assert(status == PSA_SUCCESS);
    hard_assert(public_key_length == 65);
    print_hex("Public key", public_key, public_key_length);
    printf("\n");

    /* ------------------------------------------------------------------ */
    /* Sign a message                                                      */
    /* ------------------------------------------------------------------ */
    const char *message = "Hello from Raspberry Pi Pico!";
    size_t message_length = strlen(message);

    /*
     * PSA ECDSA signatures are r || s in raw format.
     * For P-256 that is 32 + 32 = 64 bytes.
     */
    uint8_t signature[PSA_SIGN_OUTPUT_SIZE(
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
        256,
        PSA_ALG_ECDSA(PSA_ALG_SHA_256))];
    size_t signature_length = 0;

    start = time_us_64();
    status = psa_sign_message(key_id,
                              PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                              (const uint8_t *)message, message_length,
                              signature, sizeof(signature),
                              &signature_length);
    hard_assert(status == PSA_SUCCESS);
    uint64_t sign_time = (time_us_64() - start) / 1000;
    printf("Sign time: %" PRIu64 "ms\n", sign_time);
    print_hex("Signature", signature, signature_length);
    printf("\n");

    /* ------------------------------------------------------------------ */
    /* Verify the signature with the key pair                              */
    /* ------------------------------------------------------------------ */
    start = time_us_64();
    status = psa_verify_message(key_id,
                                PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                (const uint8_t *)message, message_length,
                                signature, signature_length);
    hard_assert(status == PSA_SUCCESS);
    uint64_t verify_time = (time_us_64() - start) / 1000;
    printf("Verify time (key pair): %" PRIu64 "ms\n", verify_time);
    printf("Signature valid\n\n");

    /* ------------------------------------------------------------------ */
    /* Import just the public key and verify again                         */
    /*                                                                     */
    /* This simulates the typical scenario where a verifier only holds     */
    /* the public key, not the full key pair.                              */
    /* ------------------------------------------------------------------ */
    psa_key_id_t pub_key_id;
    psa_key_attributes_t pub_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&pub_attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&pub_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&pub_attributes,
                     PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    status = psa_import_key(&pub_attributes, public_key, public_key_length,
                            &pub_key_id);
    hard_assert(status == PSA_SUCCESS);

    start = time_us_64();
    status = psa_verify_message(pub_key_id,
                                PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                (const uint8_t *)message, message_length,
                                signature, signature_length);
    hard_assert(status == PSA_SUCCESS);
    verify_time = (time_us_64() - start) / 1000;
    printf("Verify time (public key only): %" PRIu64 "ms\n", verify_time);
    printf("Signature valid (public key only)\n\n");

    /* ------------------------------------------------------------------ */
    /* Negative test: a tampered message must fail verification             */
    /* ------------------------------------------------------------------ */
    char tampered[] = "Hello from Raspberry Pi Pico!";
    tampered[0] ^= 0x01;   /* flip one bit */
    status = psa_verify_message(pub_key_id,
                                PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                (const uint8_t *)tampered, strlen(tampered),
                                signature, signature_length);
    hard_assert(status == PSA_ERROR_INVALID_SIGNATURE);
    printf("Tampered message correctly rejected\n\n");

    /* ------------------------------------------------------------------ */
    /* Clean up                                                            */
    /* ------------------------------------------------------------------ */
    psa_destroy_key(key_id);
    psa_destroy_key(pub_key_id);
    psa_reset_key_attributes(&attributes);
    psa_reset_key_attributes(&pub_attributes);

    printf("All tests passed\n");
    return 0;
}
