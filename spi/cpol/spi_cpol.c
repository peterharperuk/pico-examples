#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/time.h"

static int _sck = 2;
static int _mosi = 3;
static int _miso = 4;
static int _cs = 5;
static spi_inst_t* _spi = spi0;

int main(){
  gpio_set_function(_sck, GPIO_FUNC_SPI);
  gpio_set_function(_mosi, GPIO_FUNC_SPI);
  gpio_set_function(_miso, GPIO_FUNC_SPI);
  gpio_set_function(_cs, GPIO_FUNC_SPI);

  spi_init(_spi, 100000);
  spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

  while(1) {
    uint8_t value = 0xAA;
    (void) spi_write_blocking(_spi, &value, sizeof(value));
    sleep_ms(100);
  }
  return 0;
}
