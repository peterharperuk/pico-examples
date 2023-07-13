add_executable(hello_sleep_usb hello_sleep_usb.c)

target_link_libraries(hello_sleep_usb pico_stdlib hardware_sleep)

pico_enable_stdio_usb(hello_sleep_usb 1)
pico_enable_stdio_uart(hello_sleep_usb 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_sleep_usb)