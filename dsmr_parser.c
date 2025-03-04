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

    // Parsing all DSMR fields
    char *fields[] = {
        "0-0:1.0.0(", "1-0:1.7.0(", "1-0:2.7.0(", "1-0:32.7.0(",
        "1-0:52.7.0(", "1-0:72.7.0(", "1-0:31.7.0(", "1-0:51.7.0(", "1-0:71.7.0(",
        "1-0:1.8.0(", "1-0:2.8.0(", "0-0:96.7.21(", "0-0:96.7.9(", "1-0:32.32.0(", "1-0:32.36.0("
    };
    void *data_fields[] = {
        data->timestamp, &data->power_delivered, &data->power_returned,
        &data->voltage_l1, &data->voltage_l2, &data->voltage_l3,
        &data->current_l1, &data->current_l2, &data->current_l3,
        &data->energy_delivered_total, &data->energy_returned_total,
        &data->electricity_failures, &data->electricity_long_failures,
        &data->electricity_sags_l1, &data->electricity_swells_l1
    };
    char *formats[] = {
        "%15[^)]", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*", "%f*",
        "%d", "%d", "%d", "%d"
    };

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        char *start = strstr(raw_data, fields[i]);
        if (start) {
            sscanf(start, formats[i], data_fields[i]);
        }
    }
    
    return 0;
}
