/* gpio_control_rpi.c - Raspberry Pi implementation of GPIO handling using pigpio */
#include "gpio_control.h"
#include <pigpio.h>
#include <stdio.h>

void gpio_init() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize pigpio\n");
        return;
    }
    gpioSetMode(REQUEST_PIN, PI_OUTPUT);
    gpioWrite(REQUEST_PIN, 0); // Ensure pin starts low
}

void gpio_set_request_high() {
    gpioWrite(REQUEST_PIN, 1);
}

void gpio_set_request_low() {
    gpioWrite(REQUEST_PIN, 0);
}

void gpio_cleanup() {
    gpioWrite(REQUEST_PIN, 0); // Reset pin to low before exiting
    gpioTerminate();
}
