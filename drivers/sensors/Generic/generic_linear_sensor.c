#include "generic_linear_sensor.h"

void generic_linear_update(Generic_Linear_Sensor_t *sensor, uint16_t adc_value) {

    // 12bit ADC + 3.3V ref.
    sensor->voltage = adc_value * (3.3f / 4095.0f);

    if (adc_value < sensor->min_adc) {
        adc_value = sensor->min_adc;
    } else if (adc_value > sensor->max_adc) {
        adc_value = sensor->max_adc;
    }

    if (sensor->max_adc == sensor->min_adc) {
        sensor->displacement = sensor->min_real;
    } else {
        float scale = (sensor->max_real - sensor->min_real) / (float)(sensor->max_adc - sensor->min_adc);
        sensor->displacement = sensor->min_real + (float)(adc_value - sensor->min_adc) * scale;

        if (sensor->displacement < sensor->min_real) {
            sensor->displacement = sensor->min_real;
        } else if (sensor->displacement > sensor->max_real) {
            sensor->displacement = sensor->max_real;
        }
    }

    // Pre-Kalman raw value (physical units)
    sensor->generic_sensor.raw_value = sensor->displacement;
}


