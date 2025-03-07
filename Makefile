# Makefile for DSMR P1 Reader Cross-Compilation with pigpio and Paho MQTT

CROSS_COMPILE = arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
# Add Paho MQTT include path along with pigpio include path
CFLAGS = -Wall -Wextra -O2 -DPLATFORM_RPI -I$(HOME)/pigpio-cross/include -I$(HOME)/paho-cross/include
# Link with pigpio and Paho MQTT shared libraries
LDFLAGS = -L$(HOME)/pigpio-cross/lib -lpigpio -L$(HOME)/paho-cross/lib -lpaho-mqtt3c

TARGET = dsmr_reader

# Add mqtt_module.c to the sources list
SRCS = main.c serial.c dsmr_parser.c crc16.c gpio_control.c mqtt_module.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
