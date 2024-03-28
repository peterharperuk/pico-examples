#ifndef POWMAN_EXAMPLE_H
#define POWMAN_EXAMPLE_H

void powman_example_init(uint64_t abs_time_ms);
int powman_example_off_until_gpio_high(int gpio);
int powman_example_off_until_time(uint64_t abs_time_ms);
int powman_example_off_for_ms(uint64_t duration_ms);

#endif
