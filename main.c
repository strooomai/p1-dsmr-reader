/* main.c - DSMR P1 Reader Main Program for Raspberry Pi */
#include "serial.h"
#include "dsmr_parser.h"
#include "gpio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

int main() {
    int serial_fd;
    char buffer[BUFFER_SIZE];
    DSMRData data;

    printf("Initializing GPIO...\n");
    gpio_init();
    
    printf("Opening serial port...\n");
    serial_fd = serial_open();
    if (serial_fd == -1) {
        fprintf(stderr, "Failed to open serial port\n");
        return EXIT_FAILURE;
    }

    while (1) {
        gpio_set_request_high(); // Request new data
        usleep(500000); // Wait for response
        gpio_set_request_low();

        int bytes_read = serial_read(serial_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received data:\n%s\n", buffer);

            if (parse_dsmr_message(buffer, &data) == 0) {
                printf("Parsed Data:\n");
                printf("Timestamp: %s\n", data.timestamp);
                printf("Electricity Imported Total: %.3f kWh\n", data.electricity_imported_total);
                printf("Electricity Exported Total: %.3f kWh\n", data.electricity_exported_total);
                printf("Current Electricity Usage: %.3f kW\n", data.current_electricity_usage);
                printf("Current Electricity Delivery: %.3f kW\n", data.current_electricity_delivery);
                printf("Voltage L1: %.1f V\n", data.instantaneous_voltage_l1);
                printf("Voltage L2: %.1f V\n", data.instantaneous_voltage_l2);
                printf("Voltage L3: %.1f V\n", data.instantaneous_voltage_l3);
                printf("Current L1: %.3f A\n", data.instantaneous_current_l1);
                printf("Current L2: %.3f A\n", data.instantaneous_current_l2);
                printf("Current L3: %.3f A\n", data.instantaneous_current_l3);
                printf("Instantaneous Active Power L1 (Import): %.3f kW\n", data.instantaneous_active_power_l1_positive);
                printf("Instantaneous Active Power L2 (Import): %.3f kW\n", data.instantaneous_active_power_l2_positive);
                printf("Instantaneous Active Power L3 (Import): %.3f kW\n", data.instantaneous_active_power_l3_positive);
                printf("Instantaneous Active Power L1 (Export): %.3f kW\n", data.instantaneous_active_power_l1_negative);
                printf("Instantaneous Active Power L2 (Export): %.3f kW\n", data.instantaneous_active_power_l2_negative);
                printf("Instantaneous Active Power L3 (Export): %.3f kW\n", data.instantaneous_active_power_l3_negative);
                printf("Gas Equipment Identifier: %s\n", data.equipment_identifier_gas);
                printf("Hourly Gas Meter Reading: %.3f m³\n", data.hourly_gas_meter_reading);
            } else {
                printf("Failed to parse DSMR message.\n");
            }
        }
        sleep(1); // Wait before requesting again
    }

    printf("Cleaning up...\n");
    serial_close(serial_fd);
    gpio_cleanup();
    return EXIT_SUCCESS;
}
