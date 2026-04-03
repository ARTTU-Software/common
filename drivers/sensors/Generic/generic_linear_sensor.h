#ifndef INC_GENERIC_LINEAR_SENSOR_H
#define INC_GENERIC_LINEAR_SENSOR_H

#include <stdint.h>
#include "generic_sensor.h"

typedef struct {
    uint16_t min_adc;
    uint16_t max_adc;
    float min_real;
    float max_real;
    float displacement;
    float voltage;
    Generic_Sensor_t generic_sensor;
} Generic_Linear_Sensor_t;

void generic_linear_update(Generic_Linear_Sensor_t *sensor, uint16_t adc_value);

#endif /* INC_GENERIC_LINEAR_SENSOR_H */
