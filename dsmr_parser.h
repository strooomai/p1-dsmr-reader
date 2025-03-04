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
    float voltage_l2;
    float voltage_l3;
    float current_l1;
    float current_l2;
    float current_l3;
    float energy_delivered_total;
    float energy_returned_total;
    int electricity_failures;
    int electricity_long_failures;
    int electricity_sags_l1;
    int electricity_swells_l1;
} DSMRData;

int parse_dsmr_message(const char *raw_data, DSMRData *data);

#endif
