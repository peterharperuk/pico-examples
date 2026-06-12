#if (NOT TARGET hardware_sha256)
#    return()
#endif()

# This example uses the PSA Crpto API
add_executable(psa_sha256
        psa_sha256.c
        )
target_link_libraries(psa_sha256
        pico_stdlib
        pico_mbedtls
)
target_include_directories(psa_sha256 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
pico_add_extra_outputs(psa_sha256)
example_auto_set_url(psa_sha256)
