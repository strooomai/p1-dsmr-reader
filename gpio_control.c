/* gpio_control.h - Generic GPIO handling interface for DSMR P1 request pin */
#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <stdint.h>

#define REQUEST_PIN 17 // Change based on your setup
#define PLATFORM_RPI

void gpio_init();
void gpio_set_request_high();
void gpio_set_request_low();
void gpio_cleanup();

#endif /* GPIO_CONTROL_H */



/* gpio_control_rpi.c - Raspberry Pi implementation of GPIO handling */
#ifdef PLATFORM_RPI
#include "gpio_control.h"
#include <wiringPi.h>
#include <stdio.h>

void gpio_init() {
    if (wiringPiSetupGpio() == -1) {
        fprintf(stderr, "Failed to initialize wiringPi\n");
        return;
    }
    pinMode(REQUEST_PIN, OUTPUT);
    digitalWrite(REQUEST_PIN, LOW); // Ensure pin starts low
}

void gpio_set_request_high() {
    digitalWrite(REQUEST_PIN, HIGH);
}

void gpio_set_request_low() {
    digitalWrite(REQUEST_PIN, LOW);
}

void gpio_cleanup() {
    digitalWrite(REQUEST_PIN, LOW); // Reset pin to low before exiting
}
#endif

/* gpio_control_esp32.c - ESP32 implementation of GPIO handling */
#ifdef PLATFORM_ESP32
#include "gpio_control.h"
#include "driver/gpio.h"

void gpio_init() {
    gpio_pad_select_gpio(REQUEST_PIN);
    gpio_set_direction(REQUEST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(REQUEST_PIN, 0);
}

void gpio_set_request_high() {
    gpio_set_level(REQUEST_PIN, 1);
}

void gpio_set_request_low() {
    gpio_set_level(REQUEST_PIN, 0);
}

void gpio_cleanup() {
    gpio_set_level(REQUEST_PIN, 0);
}
#endif

/* gpio_control_linux.c - General Linux implementation of GPIO handling */
#ifdef PLATFORM_LINUX
#include "gpio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_DIRECTION "/sys/class/gpio/gpio17/direction"
#define GPIO_VALUE "/sys/class/gpio/gpio17/value"

void gpio_init() {
    int fd = open(GPIO_EXPORT, O_WRONLY);
    if (fd >= 0) {
        write(fd, "17", 2);
        close(fd);
    }
    fd = open(GPIO_DIRECTION, O_WRONLY);
    if (fd >= 0) {
        write(fd, "out", 3);
        close(fd);
    }
}

void gpio_set_request_high() {
    int fd = open(GPIO_VALUE, O_WRONLY);
    if (fd >= 0) {
        write(fd, "1", 1);
        close(fd);
    }
}

void gpio_set_request_low() {
    int fd = open(GPIO_VALUE, O_WRONLY);
    if (fd >= 0) {
        write(fd, "0", 1);
        close(fd);
    }
}

void gpio_cleanup() {
    gpio_set_request_low();
    int fd = open(GPIO_UNEXPORT, O_WRONLY);
    if (fd >= 0) {
        write(fd, "17", 2);
        close(fd);
    }
}
#endif
