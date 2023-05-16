/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdio.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"

#define KEY_SIZE 2048
#define EXPONENT 65537

#define N_HEX "A1D46FBA2318F8DCEF16C280948B1CF27966B9B47225ED2989F8D74B45BD36049C0AAB5AD0FF003553BA843C8E12782FC5873BB89A3DC84B883D25666CD22BF3ACD5B675969F8BEBFBCAC93FDD927C7442B178B10D1DFF9398E52316AAE0AF74E594650BDC3C670241D418684593CDA1A7B9DC4F20D2FDC6F66344074003E211"
#define E_HEX "010001"
#define D_HEX "589552BB4F2F023ADDDD5586D0C8FD857512D82080436678D07F984A29D892D31F1F7000FC5A39A0F73E27D885E47249A4148C8A5653EF69F91F8F736BA9F84841C2D99CD8C24DE8B72B5C9BE0EDBE23F93D731749FEA9CFB4A48DD2B7F35A2703E74AA2D4DB7DE9CEEA7D763AF0ADA7AC176C4E9A22C4CDA65CEC0C65964401"
#define P_HEX "CD083568D2D46C44C40C1FA0101AF2155E59C70B08423112AF0C1202514BBA5210765E29FF13036F56C7495894D80CF8C3BAEE2839BACBB0B86F6A2965F60DB1"
#define Q_HEX "CA0EEEA5E710E8E9811A6B846399420E3AE4A4C16647E426DDF8BBBCB11CD3F35CE2E4B6BCAD07AE2C0EC2ECBFCC601B207CDD77B5673E16382B1130BF465261"
#define DP_HEX "0D0E21C07BF434B4A83B116472C2147A11D8EB98A33CFBBCF1D275EF19D815941622435AAF3839B6C432CA53CE9E772CFBE1923A937A766FD93E96E6EDEC1DF1"
#define DQ_HEX "269CEBE6305DFEE4809377F078C814E37B45AE6677114DFC4F76F5097E1F3031D592567AC55B9B98213B40ECD54A4D2361F5FAACA1B1F51F71E4690893C4F081"
#define QP_HEX "97AC5BB885ABCA314375E9E4DB1BA4B2218C90619F61BD474F5785075ECA81750A735199A8C191FE2D3355E7CF601A70E5CABDE0E02C2538BB9FB4871540B3C1"

int main() {
    stdio_init_all();

    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_rsa_context rsa_pub;
    mbedtls_rsa_context rsa_priv;
    mbedtls_rsa_init(&rsa_pub, MBEDTLS_RSA_PKCS_V15, 0);
    mbedtls_rsa_init(&rsa_priv, MBEDTLS_RSA_PKCS_V15, 0);

    mbedtls_entropy_context entropy;
    mbedtls_entropy_init( &entropy );

    const char *pers = "mbedtls_rsa";
    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers)) != 0) {
        panic("mbedtls_ctr_drbg_seed failed");
    }

    mbedtls_mpi N, P, Q, D, E, DP, DQ, QP;
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&D);
    mbedtls_mpi_init(&E);
    mbedtls_mpi_init(&DP);
    mbedtls_mpi_init(&DQ);
    mbedtls_mpi_init(&QP);

    if ((mbedtls_mpi_read_string(&N, 16, N_HEX) != 0 || mbedtls_mpi_read_string(&E, 16, E_HEX)) != 0) {
        panic("Reading public key failed");
    }

    if(mbedtls_rsa_import(&rsa_pub, &N, NULL, NULL, NULL, &E ) != 0) {
        panic("mbedtls_rsa_import failed");
    }

    unsigned char input[] = "If you can see this message, it worked";
    unsigned char buf[512];

    if (mbedtls_rsa_pkcs1_encrypt(&rsa_pub, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PUBLIC, sizeof(input) - 1, input, buf) != 0) {
        panic("mbedtls_rsa_pkcs1_encrypt failed");  
    }

    /*for(int i = 0; i < rsa_pub.len; i++) {
        printf("%02X%s", buf[i], ( i + 1 ) % 16 == 0 ? "\r\n" : " " );
    }*/

    if (mbedtls_mpi_read_string(&P, 16, P_HEX) != 0 || mbedtls_mpi_read_string(&Q, 16, Q_HEX) != 0 ||
            mbedtls_mpi_read_string(&D, 16, D_HEX) != 0 || mbedtls_mpi_read_string(&DP, 16, DP_HEX) != 0 ||
            mbedtls_mpi_read_string(&DQ, 16, DQ_HEX) != 0 || mbedtls_mpi_read_string(&QP, 16, QP_HEX)) {
        panic("Reading private key failed");
    }

    if (mbedtls_rsa_import(&rsa_priv, &N, &P, &Q, &D, &E) != 0) {
        panic("mbedtls_rsa_import failed");
    }

    if (mbedtls_rsa_complete(&rsa_priv) != 0) {
        panic("mbedtls_rsa_complete failed");
    }

    unsigned char result[1024];
    size_t olen;
    int ret;
    if ((ret = mbedtls_rsa_pkcs1_decrypt(&rsa_priv, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PRIVATE, &olen, buf, result, sizeof(result))) != 0) {
        panic("mbedtls_rsa_pkcs1_decrypt failed %d", ret);
    }

    if (strncmp((char*)result, (const char*)input, olen) != 0) {
        panic("test failed");
    }
    printf("The decrypted result is: '%s'\n\n", result);

    mbedtls_mpi_free(&N);
    mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&D);
    mbedtls_mpi_free(&E);
    mbedtls_mpi_free(&DP);
    mbedtls_mpi_free(&DQ);
    mbedtls_mpi_free(&QP);

    mbedtls_entropy_free(&entropy);
    mbedtls_rsa_free(&rsa_pub);
    mbedtls_rsa_free(&rsa_priv);
    mbedtls_ctr_drbg_free(&ctr_drbg);

    return 0;
}