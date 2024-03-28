add_executable(powman_gpio
        powman_gpio.c
        )
target_link_libraries(powman_gpio
        pico_stdlib
        powman_example)

# create map/bin/hex file etc.
pico_add_extra_outputs(powman_gpio)

# add url via pico_set_program_url
example_auto_set_url(powman_gpio)
