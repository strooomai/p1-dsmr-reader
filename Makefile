/* Makefile for DSMR P1 Reader on Raspberry Pi */

CC = gcc
CFLAGS = -Wall -Wextra -O2 -DPLATFORM_RPI
LDFLAGS = -lwiringPi
TARGET = dsmr_reader

SRCS = main.c serial.c dsmr_parser.c crc16.c gpio_control_rpi.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
