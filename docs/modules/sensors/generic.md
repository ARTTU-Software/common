---
title: Common - Generic Sensors
description: Base sensor structs and linear mapping helper.
---

# Generic Sensor Base

These helpers provide shared storage and linear scaling used by most sensor drivers.

## Types
- Generic_Kalman_Settings_t: lightweight Kalman state and covariance storage.
- Generic_Sensor_t: raw and filtered values, kalman status, port, and settings.
- Generic_Linear_Sensor_t: ADC scaling from min/max counts to real units.

## Functions
#### `float generic_sensor_encode(const Generic_Sensor_t* sensor, float value)`
Declared in the header but no implementation exists in common. Boards should provide their own or avoid calling it.

#### `void generic_linear_update(Generic_Linear_Sensor_t* sensor, uint16_t adc_value)`
Converts raw ADC counts to voltage (3.3V, 12-bit), clamps the ADC range, and updates displacement and raw_value.

### Usage
```c
#include "generic_sensor.h"
#include "generic_linear_sensor.h"

void generic_example(void) {
    Generic_Linear_Sensor_t s = {0};
    s.min_adc = 1000;
    s.max_adc = 3000;
    s.min_real = 0.0f;
    s.max_real = 100.0f;
    generic_linear_update(&s, 2048);
}
```
