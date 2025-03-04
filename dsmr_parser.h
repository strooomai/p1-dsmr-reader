/* dsmr_parser.h - DSMR message parsing for Raspberry Pi DSMR P1 reader */
#ifndef DSMR_PARSER_H
#define DSMR_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_DSMR_MESSAGE_SIZE 2048

// Structure to store parsed DSMR v5 data
typedef struct {
    char timestamp[16];
    float electricity_imported_total;
    float electricity_exported_total;
    float electricity_used_tariff_1;
    float electricity_used_tariff_2;
    float electricity_delivered_tariff_1;
    float electricity_delivered_tariff_2;
    float current_electricity_usage;
    float current_electricity_delivery;
    int long_power_failure_count;
    int short_power_failure_count;
    int voltage_sag_l1_count;
    int voltage_sag_l2_count;
    int voltage_sag_l3_count;
    int voltage_swell_l1_count;
    int voltage_swell_l2_count;
    int voltage_swell_l3_count;
    float instantaneous_voltage_l1;
    float instantaneous_voltage_l2;
    float instantaneous_voltage_l3;
    float instantaneous_current_l1;
    float instantaneous_current_l2;
    float instantaneous_current_l3;
    float instantaneous_active_power_l1_positive;
    float instantaneous_active_power_l2_positive;
    float instantaneous_active_power_l3_positive;
    float instantaneous_active_power_l1_negative;
    float instantaneous_active_power_l2_negative;
    float instantaneous_active_power_l3_negative;
    char equipment_identifier_gas[32];
    float hourly_gas_meter_reading;
} DSMRData;

int parse_dsmr_message(const char *raw_data, DSMRData *data);

#endif
