#include "steering.h"

void steering_sensor_init(Generic_Linear_Sensor_t* sensor, uint16_t min_adc, uint16_t max_adc, float min_angle, float max_angle, uint8_t port, Generic_Kalman_Settings_t kalman_settings){
    sensor->min_adc = min_adc;
    sensor->max_adc = max_adc;
    sensor->min_real = min_angle;
    sensor->max_real = max_angle;
    sensor->generic_sensor.port = port;
    sensor->generic_sensor.kalman_settings = kalman_settings;
}

void steering_sensor_update(Generic_Linear_Sensor_t* sensor, uint16_t filtered_value){
    generic_linear_update(sensor, filtered_value);
}

int16_t steering_sensor_decode(uint8_t high_byte, uint8_t low_byte){
    // Combine bytes into signed 16-bit value (two's complement)
    int16_t value = (int16_t)((high_byte << 8) | low_byte);
    return value;
}

void steering_sensor_encode(int16_t angle, uint8_t* high_byte, uint8_t* low_byte){
    // Split signed 16-bit value into two bytes
    *high_byte = (uint8_t)((angle >> 8) & 0xFF);
    *low_byte = (uint8_t)(angle & 0xFF);
}
