add_executable(mbedtls_rsa
    rsa.c
)
target_link_libraries(mbedtls_rsa
    pico_stdlib
    pico_cyw43_arch_none
    pico_mbedtls
)
target_include_directories(mbedtls_rsa PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)
pico_add_extra_outputs(mbedtls_rsa)
example_auto_set_url(mbedtls_rsa)