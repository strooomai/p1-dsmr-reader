/* crc16.c - CRC16 checksum calculation implementation for DSMR messages */
#include "crc16.h"

uint16_t crc16_calculate(const char *data, size_t length) {
    uint16_t crc = 0x0000;  // initial value
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint8_t)data[i];  // XOR byte into least significant byte of CRC
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}


