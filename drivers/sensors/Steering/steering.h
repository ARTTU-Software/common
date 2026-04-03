#ifndef INC_STEERING_H
#define INC_STEERING_H

#include <stdint.h>
#include "generic_linear_sensor.h"

typedef struct {
    int16_t angle;            // current angle in degrees (positive = left, negative = right, 0 = center)
    float min_val;            // ADC value at hard left
    float mid_val;            // ADC value at center (0 degrees)
    float max_val;            // ADC value at hard right
    float max_left_angle;     // Maximum physical angle for a left turn (always positive).
    float max_right_angle;    // Maximum physical angle for a right turn (always stored as a negative value).
    Generic_Sensor_t generic_sensor;
} Steering_Sensor_t;

void steering_sensor_update(Steering_Sensor_t* sensor, uint16_t filtered_value);

/**
 * @brief Initializes the steering sensor driver.
 * 
 * @param sensor Pointer to the Steering_Sensor_t structure.
 * @param min_val The raw ADC value at maximum left turn.
 * @param mid_val The raw ADC value at the center position.
 * @param max_val The raw ADC value at maximum right turn.
 * @param max_left_angle The maximum physical steering angle to the left in degrees. Should be a positive value.
 * @param max_right_angle The maximum physical steering angle to the right in degrees. Should be passed as a positive magnitude (e.g., 40.0 for 40 degrees right). The driver will store it as a negative value.
 * @param port The ADC channel/port index.
 * @param kalman_settings The Kalman filter settings for this sensor.
 */
void steering_sensor_init(Steering_Sensor_t* sensor, uint16_t min_val, uint16_t mid_val, uint16_t max_val, float max_left_angle, float max_right_angle, uint8_t port, Generic_Kalman_Settings_t kalman_settings);

int16_t steering_sensor_decode(uint8_t high_byte, uint8_t low_byte);
void steering_sensor_encode(int16_t angle, uint8_t* high_byte, uint8_t* low_byte);

#endif /* INC_STEERING_H */