#include "dsmr_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *normalize_telegram(const char *telegram, int use_crlf) {
    size_t len = strlen(telegram);
    char *temp = malloc(len + 1);
    if (!temp) return NULL;
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (telegram[i] != '\r') {
            temp[j++] = telegram[i];
        }
    }
    temp[j] = '\0';

    if (use_crlf) {
        size_t count_nl = 0;
        for (size_t i = 0; i < j; i++) {
            if (temp[i] == '\n') count_nl++;
        }
        size_t new_len = j + count_nl;
        char *result = malloc(new_len + 1);
        if (!result) {
            free(temp);
            return NULL;
        }
        size_t k = 0;
        for (size_t i = 0; i < j; i++) {
            if (temp[i] == '\n') {
                result[k++] = '\r';
                result[k++] = '\n';
            } else {
                result[k++] = temp[i];
            }
        }
        result[k] = '\0';
        free(temp);
        return result;
    }
    return temp;
}

int parse_dsmr_message(const char *raw_data, DSMRData *data) {
    if (!raw_data || !data) {
        fprintf(stderr, "Error: Null input to parse_dsmr_message.\n");
        return -1;
    }

    memset(data, 0, sizeof(DSMRData));

    const char *fields[] = {
        "1-3:0.2.8(", "0-0:1.0.0(", "0-0:96.1.1(", "0-1:96.1.0(", "0-0:96.14.0(", // 5
        "1-0:1.8.1(", "1-0:1.8.2(", "1-0:2.8.1(", "1-0:2.8.2(",                    // 9
        "1-0:1.7.0(", "1-0:2.7.0(",                                               // 11
        "0-0:96.7.21(", "0-0:96.7.9(", "1-0:99.97.0(",                            // 14
        "1-0:32.32.0(", "1-0:52.32.0(", "1-0:72.32.0(",                           // 17
        "1-0:32.36.0(", "1-0:52.36.0(", "1-0:72.36.0(",                           // 20
        "1-0:32.7.0(", "1-0:52.7.0(", "1-0:72.7.0(",                              // 23
        "1-0:31.7.0(", "1-0:51.7.0(", "1-0:71.7.0(",                              // 26
        "1-0:21.7.0(", "1-0:41.7.0(", "1-0:61.7.0(",                              // 29
        "1-0:22.7.0(", "1-0:42.7.0(", "1-0:62.7.0(",                              // 32
        "0-1:24.1.0(", "0-1:24.2.1(", "0-0:96.13.0("                              // 35
    };

    void *data_fields[] = {
        &data->dsmr_version, data->timestamp, data->equipment_identifier, data->equipment_identifier_gas, &data->electricity_active_tariff,
        &data->electricity_used_tariff_1, &data->electricity_used_tariff_2,
        &data->electricity_delivered_tariff_1, &data->electricity_delivered_tariff_2,
        &data->current_electricity_usage, &data->current_electricity_delivery,
        &data->long_power_failure_count, &data->short_power_failure_count, &data->power_failure_log_count,
        &data->voltage_sag_l1_count, &data->voltage_sag_l2_count, &data->voltage_sag_l3_count,
        &data->voltage_swell_l1_count, &data->voltage_swell_l2_count, &data->voltage_swell_l3_count,
        &data->instantaneous_voltage_l1, &data->instantaneous_voltage_l2, &data->instantaneous_voltage_l3,
        &data->instantaneous_current_l1, &data->instantaneous_current_l2, &data->instantaneous_current_l3,
        &data->instantaneous_active_power_l1_positive, &data->instantaneous_active_power_l2_positive, &data->instantaneous_active_power_l3_positive,
        &data->instantaneous_active_power_l1_negative, &data->instantaneous_active_power_l2_negative, &data->instantaneous_active_power_l3_negative,
        &data->gas_meter_device_type, &data->hourly_gas_meter_reading, data->text_message
    };

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        char *start = strstr(raw_data, fields[i]);
        if (start) {
            char temp[256] = {0}; // Increased size for text_message
            start += strlen(fields[i]); // Skip the OBIS code

            if (i == 33) { // Special handling for 0-1:24.2.1 (gas reading)
                char *second_paren = strchr(start, '(');
                if (second_paren) {
                    start = second_paren + 1;
                    sscanf(start, "%255[^)]", temp);
                }
            } else {
                sscanf(start, "%255[^)]", temp);
            }

            for (int j = 0; temp[j] != '\0'; j++) {
                if (temp[j] == '*') {
                    temp[j] = '\0';
                    break;
                }
            }

            if (i == 1) { // Timestamp
                snprintf(data->timestamp, 16, "%s", temp);
            } else if (i == 2) { // Equipment Identifier
                snprintf(data->equipment_identifier, 32, "%s", temp);
            } else if (i == 3) { // Gas Equipment Identifier
                snprintf(data->equipment_identifier_gas, 32, "%s", temp);
            } else if (i == 34) { // Text Message
                snprintf(data->text_message, 256, "%s", temp);
            } else if (i == 0 || i == 4 || (i >= 11 && i <= 19) || i == 32) { // Integer fields
                sscanf(temp, "%d", (int *)data_fields[i]);
            } else { // Float fields
                sscanf(temp, "%f", (float *)data_fields[i]);
            }
        }
    }

    return 0;
}