#include "pico/btstack_flash_bank.h"
#include "hardware/flash.h"

#if TEST_FLASH_BANK_DYNAMIC
uint32_t pico_flash_bank_get_offset_hid(void) {
    static const uint8_t __bluetooth_tlv[PICO_FLASH_BANK_TOTAL_SIZE] __attribute__((aligned(FLASH_SECTOR_SIZE)));
    extern const uint8_t __flash_binary_start;
    return (__bluetooth_tlv - &__flash_binary_start);
}
#else
uint32_t pico_flash_bank_get_offset_hid(void) {
    return PICO_FLASH_BANK_STORAGE_OFFSET;
}
#endif
