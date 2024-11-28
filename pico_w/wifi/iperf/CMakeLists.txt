add_executable(picow_iperf_server_background
        picow_iperf.c
        )
target_compile_definitions(picow_iperf_server_background PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"

        CYW43_DEFAULT_PIN_WL_REG_ON=2
        CYW43_DEFAULT_PIN_WL_CS=3
        CYW43_DEFAULT_PIN_WL_DATA_OUT=4
        CYW43_DEFAULT_PIN_WL_DATA_IN=4
        CYW43_DEFAULT_PIN_WL_HOST_WAKE=4
        CYW43_DEFAULT_PIN_WL_CLOCK=5
        CYW43_PIO_CLOCK_DIV_INT=3
        )

target_include_directories(picow_iperf_server_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(picow_iperf_server_background
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        pico_lwip_iperf
        )
pico_add_extra_outputs(picow_iperf_server_background)

add_executable(picow_iperf_server_poll
        picow_iperf.c
        )
target_compile_definitions(picow_iperf_server_poll PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"

        CYW43_DEFAULT_PIN_WL_REG_ON=2
        CYW43_DEFAULT_PIN_WL_CS=3
        CYW43_DEFAULT_PIN_WL_DATA_OUT=4
        CYW43_DEFAULT_PIN_WL_DATA_IN=4
        CYW43_DEFAULT_PIN_WL_HOST_WAKE=4
        CYW43_DEFAULT_PIN_WL_CLOCK=5
        CYW43_PIO_CLOCK_DIV_INT=3
        )
target_include_directories(picow_iperf_server_poll PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(picow_iperf_server_poll
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        pico_lwip_iperf
        )
pico_add_extra_outputs(picow_iperf_server_poll)

