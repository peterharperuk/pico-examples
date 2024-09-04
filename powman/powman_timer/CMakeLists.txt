add_executable(powman_timer
        powman_timer.c
        )
target_link_libraries(powman_timer PRIVATE
        pico_stdlib
        powman_example
        hardware_adc
)

# create map/bin/hex file etc.
pico_add_extra_outputs(powman_timer)

# add url via pico_set_program_url
example_auto_set_url(powman_timer)

# We are initialising USB so we use less power when we go to sleep!
pico_enable_stdio_usb(powman_timer 1)
pico_enable_stdio_uart(powman_timer 1)
