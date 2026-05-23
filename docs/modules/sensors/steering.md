---
title: Common - Steering Sensor
description: Linear steering angle mapping and encode helpers.
---

# Steering Sensor

Linear steering angle mapping with signed 16-bit encode and decode helpers.

## Functions
#### `void steering_sensor_init(Generic_Linear_Sensor_t* sensor, uint16_t min_adc, uint16_t max_adc, float min_angle, float max_angle, uint8_t port, Generic_Kalman_Settings_t kalman_settings)`
Initializes sensor range and port.

#### `void steering_sensor_update(Generic_Linear_Sensor_t* sensor, uint16_t filtered_value)`
Updates displacement using the generic linear path.

#### `int16_t steering_sensor_decode(uint8_t high_byte, uint8_t low_byte)`
Combines two bytes into a signed angle.

#### `void steering_sensor_encode(int16_t angle, uint8_t* high_byte, uint8_t* low_byte)`
Splits a signed angle into high and low bytes.

### Usage
```c
#include "steering.h"

void steering_example(void) {
    Generic_Linear_Sensor_t s = {0};
    Generic_Kalman_Settings_t k = {0};
    steering_sensor_init(&s, 1000, 3000, -90.0f, 90.0f, 0, k);
    steering_sensor_update(&s, 2048);
}
```
