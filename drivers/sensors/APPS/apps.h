#ifndef INC_APPS_H
#define INC_APPS_H

#include "stdint.h"
#include "generic_linear_sensor.h"

void APPS_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k);
void APPS_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc);
float APPS_decode(uint8_t high_byte, uint8_t low_byte);


#endif /* INC_APPS_H */
