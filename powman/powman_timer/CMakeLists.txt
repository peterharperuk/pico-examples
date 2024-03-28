add_executable(powman_timer
        powman_timer.c
        )
target_link_libraries(powman_timer PRIVATE
        pico_stdlib
        powman_example
)

# create map/bin/hex file etc.
pico_add_extra_outputs(powman_timer)

# add url via pico_set_program_url
example_auto_set_url(powman_timer)
