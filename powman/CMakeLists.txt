add_library(powman_example INTERFACE)
target_sources(powman_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/powman_example.c
)
target_include_directories(powman_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(powman_example INTERFACE
    hardware_powman
    hardware_gpio
)

add_subdirectory(powman_timer)
add_subdirectory(powman_gpio)
