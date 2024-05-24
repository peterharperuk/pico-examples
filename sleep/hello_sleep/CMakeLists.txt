add_executable(hello_sleep_rtc
        hello_sleep_rtc.c
        )
target_link_libraries(hello_sleep_rtc
        pico_stdlib
        hardware_sleep
        )
pico_add_extra_outputs(hello_sleep_rtc)

add_executable(hello_sleep_alarm
        hello_sleep_alarm.c
        )
target_link_libraries(hello_sleep_alarm
        pico_stdlib
        hardware_sleep
        )
pico_add_extra_outputs(hello_sleep_alarm)