/* dsmr_parser.h - DSMR message parsing for Raspberry Pi DSMR P1 reader */
#ifndef DSMR_PARSER_H
#define DSMR_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_DSMR_MESSAGE_SIZE 1024

// Structure to store parsed DSMR data
typedef struct {
    char timestamp[16];
    float power_delivered;
    float power_returned;
    float voltage_l1;
    float current_l1;
} DSMRData;

int parse_dsmr_message(const char *raw_data, DSMRData *data);

#endif
