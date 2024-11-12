add_library(gatt_counter_led INTERFACE)
target_compile_definitions(gatt_counter_led INTERFACE
    LED_BLINK=1
    )
picow_bt_example(gatt_counter gatt_counter_led)
