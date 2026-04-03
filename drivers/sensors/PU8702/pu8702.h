#ifndef INC_PU8702_H
#define INC_PU8702_H

#include <stdint.h>
#include "generic_linear_sensor.h"

// This is the break pressure sensor

void pu8702_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc);
void pu8702_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k);
float pu8702_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_PU8702_H */
