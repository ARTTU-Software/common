#ifndef INC_STEERING_H
#define INC_STEERING_H

#include <stdint.h>
#include "generic_linear_sensor.h"

/**
 * @brief Initializes the steering sensor driver.
 * 
 * @param sensor Pointer to the Generic_Linear_Sensor_t structure.
 * @param min_adc The raw ADC value at minimum angle.
 * @param max_adc The raw ADC value at maximum angle.
 * @param min_angle The angle value corresponding to min_adc.
 * @param max_angle The angle value corresponding to max_adc.
 * @param port The ADC channel/port index.
 * @param kalman_settings The Kalman filter settings for this sensor.
 */
void steering_sensor_init(Generic_Linear_Sensor_t* sensor, uint16_t min_adc, uint16_t max_adc, float min_angle, float max_angle, uint8_t port, Generic_Kalman_Settings_t kalman_settings);

void steering_sensor_update(Generic_Linear_Sensor_t* sensor, uint16_t filtered_value);

int16_t steering_sensor_decode(uint8_t high_byte, uint8_t low_byte);
void steering_sensor_encode(int16_t angle, uint8_t* high_byte, uint8_t* low_byte);

#endif /* INC_STEERING_H */