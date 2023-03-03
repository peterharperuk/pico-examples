add_library(flash_bank_hid INTERFACE)
target_sources(flash_bank_hid INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/flash_bank_hid.c
)
target_compile_definitions(flash_bank_hid INTERFACE
    pico_flash_bank_get_offset_func=pico_flash_bank_get_offset_hid
    TEST_FLASH_BANK_DYNAMIC=0
)
picow_bt_example(hog_keyboard_demo flash_bank_hid)
