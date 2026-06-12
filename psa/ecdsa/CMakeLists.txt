# This example uses the PSA Crpto API
add_executable(psa_ecdsa
        psa_ecdsa.c
        )
target_link_libraries(psa_ecdsa
        pico_stdlib
        pico_mbedtls
)
target_include_directories(psa_ecdsa PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_compile_definitions(psa_ecdsa PRIVATE
        MBEDTLS_PSA_CRYPTO_CONFIG_FILE=\"psa_crypto_config.h\"
)
pico_add_extra_outputs(psa_ecdsa)
example_auto_set_url(psa_ecdsa)
