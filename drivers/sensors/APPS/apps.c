#include "apps.h"

static const float APPS_ENCODING_RESOLUTION = 1.0f; // raw

void APPS_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k){
    s->max_adc = max_adc;
    s->min_adc = min_adc;
    s->min_real = 0.0f;
    s->max_real = 100.0f; // percentage
    s->generic_sensor.port = port;
    s->generic_sensor.kalman_settings = k;
}

void APPS_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc) {
    generic_linear_update(s, raw_adc);
}

float APPS_decode(uint8_t high_byte, uint8_t low_byte) {
    uint16_t value = (high_byte << 8) | low_byte;
    return value;
}