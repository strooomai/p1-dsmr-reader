#include "serial.h"
#include "dsmr_parser.h"
#include "gpio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

static char telegram_buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

int accumulate_telegram(int serial_fd) {
    char temp[256];
    int bytes_read = serial_read(serial_fd, temp, sizeof(temp) - 1);
    if (bytes_read > 0) {
        if (buffer_index + bytes_read < BUFFER_SIZE) {
            memcpy(telegram_buffer + buffer_index, temp, bytes_read);
            buffer_index += bytes_read;
            telegram_buffer[buffer_index] = '\0';
        } else {
            fprintf(stderr, "Buffer overflow, resetting telegram buffer.\n");
            buffer_index = 0;
            telegram_buffer[0] = '\0';
            return 0;
        }
    }
    
    char *start = strchr(telegram_buffer, '/');
    if (start) {
        char *excl = strchr(start, '!');
        if (excl && strlen(excl) >= 5) {
            int valid = 1;
            for (int i = 1; i <= 4; i++) {
                char c = excl[i];
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                    valid = 0;
                    break;
                }
            }
            if (valid) return 1;
        }
    }
    return 0;
}

char *extract_telegram() {
    char *start = strchr(telegram_buffer, '/');
    if (!start) return NULL;
    
    char *excl = strchr(start, '!');
    if (!excl || strlen(excl) < 5) return NULL;
    
    size_t telegram_length = (excl - start) + 5;
    
    char *telegram = malloc(telegram_length + 1);
    if (!telegram) return NULL;
    
    memcpy(telegram, start, telegram_length);
    telegram[telegram_length] = '\0';
    
    size_t offset = start - telegram_buffer;
    size_t remaining = buffer_index - offset - telegram_length;
    memmove(telegram_buffer + offset, start + telegram_length, remaining);
    buffer_index = offset + remaining;
    telegram_buffer[buffer_index] = '\0';
    
    return telegram;
}

int main() {
    int serial_fd;
    DSMRData data;
    
    printf("Initializing GPIO...\n");
    gpio_init();
    
    printf("Opening serial port...\n");
    serial_fd = serial_open(SERIAL_PORT);
    if (serial_fd == -1) {
        fprintf(stderr, "Error: Failed to open serial port.\n");
        return EXIT_FAILURE;
    }
    
    while (1) {
        gpio_set_request_high();
        usleep(500000);
        gpio_set_request_low();
        
        if (accumulate_telegram(serial_fd)) {
            char *complete_telegram = extract_telegram();
            if (complete_telegram) {
                printf("Received complete DSMR telegram:\n%s\n", complete_telegram);
                int result = parse_dsmr_message(complete_telegram, &data);
                if (result == 0) {
                    printf("Parsed DSMR Data:\n");
                    printf("DSMR Version: %d\n", data.dsmr_version);
                    printf("Timestamp: %s\n", data.timestamp);
                    printf("Equipment ID: %s\n", data.equipment_identifier);
                    printf("Gas Equipment ID: %s\n", data.equipment_identifier_gas);
                    printf("Active Tariff: %d\n", data.electricity_active_tariff);
                    printf("Electricity Used (Tariff 1): %.3f kWh\n", data.electricity_used_tariff_1);
                    printf("Electricity Used (Tariff 2): %.3f kWh\n", data.electricity_used_tariff_2);
                    printf("Electricity Delivered (Tariff 1): %.3f kWh\n", data.electricity_delivered_tariff_1);
                    printf("Electricity Delivered (Tariff 2): %.3f kWh\n", data.electricity_delivered_tariff_2);
                    printf("Current Usage: %.3f kW\n", data.current_electricity_usage);
                    printf("Current Delivery: %.3f kW\n", data.current_electricity_delivery);
                    printf("Long Power Failures: %d\n", data.long_power_failure_count);
                    printf("Short Power Failures: %d\n", data.short_power_failure_count);
                    printf("Power Failure Log Count: %d\n", data.power_failure_log_count);
                    printf("Voltage Sags L1: %d\n", data.voltage_sag_l1_count);
                    printf("Voltage Sags L2: %d\n", data.voltage_sag_l2_count);
                    printf("Voltage Sags L3: %d\n", data.voltage_sag_l3_count);
                    printf("Voltage Swells L1: %d\n", data.voltage_swell_l1_count);
                    printf("Voltage Swells L2: %d\n", data.voltage_swell_l2_count);
                    printf("Voltage Swells L3: %d\n", data.voltage_swell_l3_count);
                    printf("Voltage L1: %.1f V\n", data.instantaneous_voltage_l1);
                    printf("Voltage L2: %.1f V\n", data.instantaneous_voltage_l2);
                    printf("Voltage L3: %.1f V\n", data.instantaneous_voltage_l3);
                    printf("Current L1: %.1f A\n", data.instantaneous_current_l1);
                    printf("Current L2: %.1f A\n", data.instantaneous_current_l2);
                    printf("Current L3: %.1f A\n", data.instantaneous_current_l3);
                    printf("Power L1 Positive: %.3f kW\n", data.instantaneous_active_power_l1_positive);
                    printf("Power L2 Positive: %.3f kW\n", data.instantaneous_active_power_l2_positive);
                    printf("Power L3 Positive: %.3f kW\n", data.instantaneous_active_power_l3_positive);
                    printf("Power L1 Negative: %.3f kW\n", data.instantaneous_active_power_l1_negative);
                    printf("Power L2 Negative: %.3f kW\n", data.instantaneous_active_power_l2_negative);
                    printf("Power L3 Negative: %.3f kW\n", data.instantaneous_active_power_l3_negative);
                    printf("Gas Device Type: %d\n", data.gas_meter_device_type);
                    printf("Gas Reading: %.3f m3\n", data.hourly_gas_meter_reading);
                    printf("Text Message: %s\n", data.text_message);
                } else {
                    fprintf(stderr, "Error: Failed to parse DSMR message. Error Code: %d\n", result);
                }
                free(complete_telegram);
            }
        }
        sleep(1);
    }
    
    printf("Cleaning up...\n");
    serial_close(serial_fd);
    gpio_cleanup();
    return EXIT_SUCCESS;
}