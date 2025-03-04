# Makefile for DSMR P1 Reader Cross-Compilation with pigpio

CROSS_COMPILE = arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
CFLAGS = -Wall -Wextra -O2 -DPLATFORM_RPI -I$(HOME)/pigpio-cross/include
LDFLAGS = -L$(HOME)/pigpio-cross/lib -lpigpio

TARGET = dsmr_reader

SRCS = main.c serial.c dsmr_parser.c crc16.c gpio_control.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
