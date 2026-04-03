#include "steering.h"

void steering_sensor_init(Steering_Sensor_t* sensor, uint16_t min_val, uint16_t mid_val, uint16_t max_val, float max_left_angle, float max_right_angle, uint8_t port, Generic_Kalman_Settings_t kalman_settings){
    sensor->min_val = (float)min_val;
    sensor->mid_val = (float)mid_val;
    sensor->max_val = (float)max_val;
    // Ensure left is positive, right is negative
    sensor->max_left_angle = (max_left_angle < 0.0f) ? -max_left_angle : max_left_angle;
    sensor->max_right_angle = (max_right_angle > 0.0f) ? -max_right_angle : max_right_angle;

    sensor->angle = 0;
    sensor->generic_sensor.port = port;
    sensor->generic_sensor.kalman_settings = kalman_settings;
    sensor->generic_sensor.filtered_value = (float)mid_val;
}

void steering_sensor_update(Steering_Sensor_t* sensor, uint16_t filtered_value){
    // Update generic sensor filtered value for debugging
    sensor->generic_sensor.filtered_value = (float)filtered_value;

    float val = (float)filtered_value;
    float angle_f = 0.0f;

    // Clamp input to calibrated range
    if (val < sensor->min_val) val = sensor->min_val;
    if (val > sensor->max_val) val = sensor->max_val;

    // Map to angle
    if (val < sensor->mid_val) {
        // Left turn (Positive angle)
        // Map [min_val, mid_val] -> [max_left_angle, 0]
        float range = sensor->mid_val - sensor->min_val;
        if (range > 0.1f) {
            float ratio = (sensor->mid_val - val) / range;
            angle_f = ratio * sensor->max_left_angle;
        }
    } else {
        // Right turn (Negative angle)
        // Map [mid_val, max_val] -> [0, max_right_angle]
        float range = sensor->max_val - sensor->mid_val;
        if (range > 0.1f) {
            float ratio = (val - sensor->mid_val) / range;
            angle_f = ratio * sensor->max_right_angle;
        }
    }

    // Assign to angle (int16_t)
    sensor->angle = (int16_t)angle_f;
}

int16_t steering_sensor_decode(uint8_t high_byte, uint8_t low_byte){
    // Combine bytes into signed 16-bit value (two's complement)
    int16_t value = (int16_t)((high_byte << 8) | low_byte);
    return value;
}

void steering_sensor_encode(int16_t angle, uint8_t* high_byte, uint8_t* low_byte){
    // Split signed 16-bit value into two bytes
    *high_byte = (uint8_t)((angle >> 8) & 0xFF);
    *low_byte = (uint8_t)(angle & 0xFF);
}
