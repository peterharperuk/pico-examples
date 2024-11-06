add_executable(slave_mem_i2c
    slave_mem_i2c.c
    ${CMAKE_CURRENT_LIST_DIR}/../../pio/i2c/pio_i2c.c

    )
target_link_libraries(slave_mem_i2c
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    pico_multicore
    hardware_pio
    )
target_include_directories(slave_mem_i2c PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../pio/i2c)

pico_generate_pio_header(slave_mem_i2c ${CMAKE_CURRENT_LIST_DIR}/../../pio/i2c/i2c.pio)

pico_add_extra_outputs(slave_mem_i2c)
example_auto_set_url(slave_mem_i2c)
