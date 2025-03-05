/* dsmr_parser.c - DSMR message parsing implementation for Raspberry Pi DSMR P1 reader */
#include "dsmr_parser.h"
#include "crc16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to normalize the telegram's newline characters.
// If use_crlf is non-zero, each LF is replaced with CR+LF; otherwise, all CRs are removed.
char *normalize_telegram(const char *telegram, int use_crlf) {
    // First, remove all '\r' characters.
    size_t len = strlen(telegram);
    char *temp = malloc(len + 1);
    if (!temp)
        return NULL;
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (telegram[i] != '\r') {
            temp[j++] = telegram[i];
        }
    }
    temp[j] = '\0';

    // If CRLF mode is desired, convert each '\n' into "\r\n".
    if (use_crlf) {
        // Count how many newlines to determine new length.
        size_t count_nl = 0;
        for (size_t i = 0; i < j; i++) {
            if (temp[i] == '\n')
                count_nl++;
        }
        size_t new_len = j + count_nl; // each \n gets an extra char
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
    } else {
        // Otherwise, just return the temp buffer (which now only contains LF).
        return temp;
    }
}




int parse_dsmr_message(const char *raw_data, DSMRData *data) {
    if (!raw_data || !data) {
        fprintf(stderr, "Error: Null input to parse_dsmr_message.\n");
        return -1;
    }

    // Initialize all fields to default values
    memset(data, 0, sizeof(DSMRData));

    #include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to normalize the telegram's newline characters.
// If use_crlf is non-zero, each LF is replaced with CR+LF; otherwise, all CRs are removed.
char *normalize_telegram(const char *telegram, int use_crlf) {
    // First, remove all '\r' characters.
    size_t len = strlen(telegram);
    char *temp = malloc(len + 1);
    if (!temp)
        return NULL;
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (telegram[i] != '\r') {
            temp[j++] = telegram[i];
        }
    }
    temp[j] = '\0';

    // If CRLF mode is desired, convert each '\n' into "\r\n".
    if (use_crlf) {
        // Count how many newlines to determine new length.
        size_t count_nl = 0;
        for (size_t i = 0; i < j; i++) {
            if (temp[i] == '\n')
                count_nl++;
        }
        size_t new_len = j + count_nl; // each \n gets an extra char
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
    } else {
        // Otherwise, just return the temp buffer (which now only contains LF).
        return temp;
    }
}


    // Define OBIS codes and corresponding fields
    const char *fields[] = {
        "0-0:1.0.0(", "1-0:1.8.1(", "1-0:1.8.2(", "1-0:2.8.1(", "1-0:2.8.2(",
        "1-0:1.7.0(", "1-0:2.7.0(", "1-0:32.7.0(", "1-0:52.7.0(", "1-0:72.7.0(",
        "1-0:31.7.0(", "1-0:51.7.0(", "1-0:71.7.0(", "0-1:24.2.1("
    };

    void *data_fields[] = {
        data->timestamp, &data->electricity_used_tariff_1, &data->electricity_used_tariff_2,
        &data->electricity_delivered_tariff_1, &data->electricity_delivered_tariff_2,
        &data->current_electricity_usage, &data->current_electricity_delivery,
        &data->instantaneous_voltage_l1, &data->instantaneous_voltage_l2, &data->instantaneous_voltage_l3,
        &data->instantaneous_current_l1, &data->instantaneous_current_l2, &data->instantaneous_current_l3,
        &data->hourly_gas_meter_reading
    };

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        char *start = strstr(raw_data, fields[i]);
        if (start) {
            char temp[32] = {0}; // Initialize temp buffer
            sscanf(start, "%*[^0123456789]%31[^)]", temp);
            
            // Strip out unit suffix (*V, *A, *kWh, *m3)
            for (int j = 0; temp[j] != '\0'; j++) {
                if (temp[j] == '*') {
                    temp[j] = '\0';
                    break;
                }
            }
            
            // Convert extracted value into correct format
            if (i == 0) {
                snprintf((char *)data_fields[i], 32, "%s", temp); // Handle timestamp as string
            } else {
                sscanf(temp, "%f", (float *)data_fields[i]);
            }
        }
    }

    return 0;
}
