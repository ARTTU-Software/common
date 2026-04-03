#ifndef INC_GE1935_H
#define INC_GE1935_H

#include <stdint.h>
#include "generic_linear_sensor.h"

typedef struct {
    Generic_Sensor_t generic_sensor;
    float v_ref;       // reference voltage
    float r_fixed;     // series resistor in divider (need to put on cable)
    uint16_t temperature;
} GE1935_t;

void ge1935_init(GE1935_t* s,
                 uint8_t port,
                 float v_ref,
                 float r_fixed,
                 Generic_Kalman_Settings_t k);

void ge1935_update(GE1935_t* s, uint16_t filtered_adc);
float ge1935_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_GE1935_H */
