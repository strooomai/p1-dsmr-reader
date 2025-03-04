/* gpio_control.h - GPIO handling for DSMR P1 request pin on Raspberry Pi */
#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <stdint.h>

#define REQUEST_PIN 17 // Change based on your setup

void gpio_init();
void gpio_set_request_high();
void gpio_set_request_low();
void gpio_cleanup();

#endif /* GPIO_CONTROL_H */
