#include "serial.h"
#include "dsmr_parser.h"
#include "gpio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

// Buffer for accumulating incoming serial data.
static char telegram_buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

/**
 * Append new serial data to the telegram buffer.
 * Returns 1 if a complete telegram is available, 0 otherwise.
 */
int accumulate_telegram(int serial_fd) {
    char temp[256];
    int bytes_read = serial_read(serial_fd, temp, sizeof(temp) - 1);
    if (bytes_read > 0) {
        // Prevent buffer overflow.
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
    
    // Check for a complete telegram:
    // It should start with '/' and contain a '!' followed by at least 4 hex digits.
    char *start = strchr(telegram_buffer, '/');
    if (start) {
        char *excl = strchr(start, '!');
        if (excl && strlen(excl) >= 5) {
            // Verify that the 4 characters following '!' are valid hex digits.
            int valid = 1;
            for (int i = 1; i <= 4; i++) {
                char c = excl[i];
                if (!((c >= '0' && c <= '9') ||
                      (c >= 'A' && c <= 'F') ||
                      (c >= 'a' && c <= 'f'))) {
                    valid = 0;
                    break;
                }
            }
            if (valid)
                return 1;
        }
    }
    return 0;
}

/**
 * Extracts a complete telegram from the buffer.
 * Returns a dynamically allocated string containing the telegram.
 * Caller is responsible for freeing the returned string.
 */
char *extract_telegram() {
    char *start = strchr(telegram_buffer, '/');
    if (!start)
        return NULL;
    
    char *excl = strchr(start, '!');
    if (!excl || strlen(excl) < 5)
        return NULL;  // Telegram not complete.
    
    // The telegram spans from the start up to and including the 4 hex digits after '!'
    size_t telegram_length = (excl - start) + 5;
    
    char *telegram = malloc(telegram_length + 1);
    if (!telegram)
        return NULL;
    
    memcpy(telegram, start, telegram_length);
    telegram[telegram_length] = '\0';
    
    // Remove the extracted telegram from the buffer.
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
        gpio_set_request_high(); // Request new data
        usleep(500000);          // Wait for response
        gpio_set_request_low();
        
        // Accumulate data until a complete telegram is available.
        if (accumulate_telegram(serial_fd)) {
            char *complete_telegram = extract_telegram();
            if (complete_telegram) {
                printf("Received complete DSMR telegram:\n%s\n", complete_telegram);
                int result = parse_dsmr_message(complete_telegram, &data);
                if (result == 0) {
                    printf("Parsed DSMR Data:\n");
                    printf("Timestamp: %s\n", data.timestamp);
                    printf("Electricity Used (Tariff 1): %.3f kWh\n", data.electricity_used_tariff_1);
                    // ... Print other fields as needed ...
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
