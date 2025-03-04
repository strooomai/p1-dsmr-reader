/* crc16.h - CRC16 checksum calculation for DSMR messages */
#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>
#include <stddef.h>

uint16_t crc16_calculate(const char *data, size_t length);

#endif /* CRC16_H */
