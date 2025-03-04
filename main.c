/* main.c - DSMR P1 Reader Main Program for Raspberry Pi */
#include "serial.h"
#include "dsmr_parser.h"
#include "gpio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

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
                printf("Power Delivered: %.3f kW\n", data.power_delivered);
                printf("Power Returned: %.3f kW\n", data.power_returned);
                printf("Voltage L1: %.1f V\n", data.voltage_l1);
                printf("Current L1: %.3f A\n", data.current_l1);
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
