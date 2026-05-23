---
title: Common - APPS
description: Accelerator pedal position sensor helper.
---

# APPS

Wrapper around Generic_Linear_Sensor_t for accelerator pedal position scaling (0 to 100 percent).

## Functions
#### `void APPS_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k)`
Initializes the linear range and sensor metadata.

#### `void APPS_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc)`
Updates the linear sensor with a raw ADC reading.

#### `float APPS_decode(uint8_t high_byte, uint8_t low_byte)`
Combines two bytes into a raw value.

### Usage
```c
#include "apps.h"

void apps_example(void) {
    Generic_Linear_Sensor_t apps = {0};
    Generic_Kalman_Settings_t k = {0};
    APPS_init(&apps, 1000, 3000, 0, k);
    APPS_update(&apps, 2000);
}
```
