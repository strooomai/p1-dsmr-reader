#ifndef DSMR_PARSER_H
#define DSMR_PARSER_H

typedef struct {
    char timestamp[16];
    char equipment_identifier[32];  // Electricity Meter ID
    char equipment_identifier_gas[32];  // Gas Meter ID
    int electricity_active_tariff;
    float electricity_imported_total;  // Not used in this telegram
    float electricity_exported_total;  // Not used in this telegram
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
    char text_message[256];  // Text messages
    int gas_meter_device_type;
    float hourly_gas_meter_reading;
    int dsmr_version;  // DSMR version
    int power_failure_log_count;  // Count of power failure events
} DSMRData;

int parse_dsmr_message(const char *raw_data, DSMRData *data);
char *normalize_telegram(const char *telegram, int use_crlf);

#endif