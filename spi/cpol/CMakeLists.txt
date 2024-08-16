add_executable(spi_cpol
        spi_cpol.c
        )

# pull in common dependencies and additional spi hardware support
target_link_libraries(spi_cpol pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(spi_cpol)

# add url via pico_set_program_url
example_auto_set_url(spi_cpol)
