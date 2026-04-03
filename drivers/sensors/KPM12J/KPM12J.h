#ifndef INC_KPM12J_H
#define INC_KPM12J_H

#include <stdint.h>
#include "generic_linear_sensor.h"

// This is the suspension travel sensor

void kpm12j_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc);
void kpm12j_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k);
float kpm12j_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_KPM12J_H */
