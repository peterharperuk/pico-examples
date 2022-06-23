add_executable(picow_dns_server_test_poll
    picow_dns_server_test.c
    dnsserver.c
)
target_include_directories(picow_dns_server_test_poll PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
)
target_link_libraries(picow_dns_server_test_poll
    pico_cyw43_arch_lwip_poll
    pico_stdlib
)
target_compile_definitions(picow_dns_server_test_poll PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
pico_add_extra_outputs(picow_dns_server_test_poll)
