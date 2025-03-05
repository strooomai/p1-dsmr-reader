/* serial.h - Serial port handling for Raspberry Pi DSMR P1 reader */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SERIAL_PORT "/dev/ttyUSB0" // Change based on your setup
#define BAUD_RATE B115200

int serial_open();
void serial_close(int fd);
int serial_read(int fd, char *buffer, size_t length);
int serial_write(int fd, const char *data, size_t length);

#endif