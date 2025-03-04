/* dsmr_parser.c - DSMR message parsing implementation for Raspberry Pi DSMR P1 reader */
#include "dsmr_parser.h"
#include "crc16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_dsmr_message(const char *raw_data, DSMRData *data) {
    if (!raw_data || !data) {
        return -1;
    }

    // Initialize all fields to default values
    memset(data, 0, sizeof(DSMRData));

    // Locate checksum delimiter '!' and extract the received CRC
    char *crc_pos = strrchr(raw_data, '!');
    if (!crc_pos || strlen(crc_pos) < 5) {
        return -1; // Invalid message format
    }
    uint16_t received_crc;
    sscanf(crc_pos + 1, "%4hx", &received_crc);

    // Compute CRC for the message (excluding the CRC itself)
    size_t data_length = crc_pos - raw_data;
    uint16_t computed_crc = crc16_calculate(raw_data, data_length);
    
    if (computed_crc != received_crc) {
        return -2; // CRC mismatch, invalid message
    }

    // Define DSMR v5 OBIS codes and map them to structure fields
    char *fields[] = {
        "0-0:1.0.0(", "1-0:1.8.0(", "1-0:2.8.0(", "1-0:1.8.1(", "1-0:1.8.2(",
        "1-0:2.8.1(", "1-0:2.8.2(", "1-0:1.7.0(", "1-0:2.7.0(", 
        "0-0:96.7.9(", "0-0:96.7.21(", "1-0:32.32.0(", "1-0:52.32.0(", "1-0:72.32.0(",
        "1-0:32.36.0(", "1-0:52.36.0(", "1-0:72.36.0(", 
        "1-0:32.7.0(", "1-0:52.7.0(", "1-0:72.7.0(", 
        "1-0:31.7.0(", "1-0:51.7.0(", "1-0:71.7.0(", 
        "1-0:21.7.0(", "1-0:41.7.0(", "1-0:61.7.0(", 
        "1-0:22.7.0(", "1-0:42.7.0(", "1-0:62.7.0(", 
        "0-1:96.1.0(", "0-1:24.2.1("
    };

    void *data_fields[] = {
        data->timestamp, &data->electricity_imported_total, &data->electricity_exported_total,
        &data->electricity_used_tariff_1, &data->electricity_used_tariff_2,
        &data->electricity_delivered_tariff_1, &data->electricity_delivered_tariff_2,
        &data->current_electricity_usage, &data->current_electricity_delivery,
        &data->long_power_failure_count, &data->short_power_failure_count,
        &data->voltage_sag_l1_count, &data->voltage_sag_l2_count, &data->voltage_sag_l3_count,
        &data->voltage_swell_l1_count, &data->voltage_swell_l2_count, &data->voltage_swell_l3_count,
        &data->instantaneous_voltage_l1, &data->instantaneous_voltage_l2, &data->instantaneous_voltage_l3,
        &data->instantaneous_current_l1, &data->instantaneous_current_l2, &data->instantaneous_current_l3,
        &data->instantaneous_active_power_l1_positive, &data->instantaneous_active_power_l2_positive, &data->instantaneous_active_power_l3_positive,
        &data->instantaneous_active_power_l1_negative, &data->instantaneous_active_power_l2_negative, &data->instantaneous_active_power_l3_negative,
        data->equipment_identifier_gas, &data->hourly_gas_meter_reading
    };

    char *formats[] = {
        "%15[^)]", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%d", "%d", "%d", "%d", "%d", "%d", "%d", "%d",
        "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%31[^)]", "%f*"
    };

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        char *start = strstr(raw_data, fields[i]);
        if (start) {
            sscanf(start, formats[i], data_fields[i]);
        }
    }
    
    return 0;
}
