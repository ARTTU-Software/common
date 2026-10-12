#include "KPM12J.h"

static const float KPM12J_ENCODING_RESOLUTION = 100.0f; // mm -> encoded units (0.01 mm resolution, max 655.35 mm)

void kpm12j_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, float max_real, uint8_t port, Generic_Kalman_Settings_t k){
    s->max_adc = max_adc;
    s->min_adc = min_adc;
    s->min_real = 0.0f;
    s->max_real = max_real;                  // range in mm
    s->generic_sensor.port = port;
    s->generic_sensor.kalman_settings = k;
}

uint16_t kpm12j_encode(float value) {
    float encoded = value * KPM12J_ENCODING_RESOLUTION;
    if (encoded < 0.0f) encoded = 0.0f;
    if (encoded > 65535.0f) encoded = 65535.0f;
    return (uint16_t)(encoded + 0.5f);
}

void kpm12j_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc) {
    generic_linear_update(s, raw_adc);
}


float kpm12j_decode(uint8_t high_byte, uint8_t low_byte){
    uint16_t value = (uint16_t)(((uint16_t)high_byte << 8) | (uint16_t)low_byte);
    return (float)value / KPM12J_ENCODING_RESOLUTION;
}