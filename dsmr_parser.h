typedef struct {
    char timestamp[16];
    char equipment_identifier[32];  // Electricity Meter ID
    char equipment_identifier_gas[32];  // Gas Meter ID
    int electricity_active_tariff;
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
    char text_message[256];  // New field for text messages
    int gas_meter_device_type;
    float hourly_gas_meter_reading;
} DSMRData;
