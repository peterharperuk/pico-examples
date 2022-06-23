# FIXME: the stuff done in this section at the top should probably be handled in pico-sdk instead
# and can no doubt be done in a cleaner way than this
set(MBEDTLS_TARGET_PREFIX pico_)
add_compile_definitions(MBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_config.h")
option(ENABLE_PROGRAMS OFF)
option(ENABLE_TESTING OFF)
add_subdirectory(${MBEDTLS_DIR} MBEDTLS)

add_executable(picow_tls_client
        picow_tls_client.c
        )
target_compile_definitions(picow_tls_client PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(picow_tls_client PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(picow_tls_client
        #pico_cyw43_arch_lwip_threadsafe_background
        pico_cyw43_arch_lwip_poll
        pico_lwip_mbedtls
        pico_mbedtls
        pico_stdlib
        )
pico_add_extra_outputs(picow_tls_client)

pico_enable_stdio_usb(picow_tls_client 1)
pico_enable_stdio_uart(picow_tls_client 0)
