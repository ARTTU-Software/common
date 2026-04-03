#include "pu8702.h"

static const float PU8702_ENCODING_RESOLUTION = 100.0f; // bar -> encoded units

void pu8702_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k){
    s->max_adc = max_adc;
    s->min_adc = min_adc;
    s->min_real = 0.0f;
    s->max_real = 100.0f;                 // range in bar
    s->generic_sensor.port = port;
    s->generic_sensor.kalman_settings = k;
}

uint16_t pu8702_encode(float value) {
    float encoded = value * PU8702_ENCODING_RESOLUTION;
    if (encoded < 0.0f) encoded = 0.0f;
    if (encoded > 65535.0f) encoded = 65535.0f;
    return (uint16_t)(encoded + 0.5f);
}

void pu8702_update(Generic_Linear_Sensor_t* s, uint16_t filtered_adc) {
    generic_linear_update(s, filtered_adc);
}


float pu8702_decode(uint8_t high_byte, uint8_t low_byte){
    uint16_t value = (high_byte << 8) | low_byte;
    return value / PU8702_ENCODING_RESOLUTION;
}