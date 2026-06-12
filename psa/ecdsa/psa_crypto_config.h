#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

/* ChaCha20-Poly1305 */
#define PSA_WANT_KEY_TYPE_CHACHA20        1
#define PSA_WANT_ALG_CHACHA20_POLY1305    1

/* ECDSA mit secp384r1 */
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR    1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY  1
#define PSA_WANT_ECC_SECP_R1_384          1
#define PSA_WANT_ALG_ECDSA                1
#define PSA_WANT_HASH_SHA384              1
/* HKDF-SHA256 */
#define PSA_WANT_ALG_HKDF                 1
#define PSA_WANT_HASH_SHA256              1

#endif
