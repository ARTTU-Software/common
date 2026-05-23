---
title: Common - KPM12J
description: Suspension travel sensor helper.
---

# KPM12J Suspension Sensor

Linear suspension travel sensor with mm scaling.

## Functions
#### `void kpm12j_init(Generic_Linear_Sensor_t* s, uint16_t min_adc, uint16_t max_adc, uint8_t port, Generic_Kalman_Settings_t k)`
Initializes the range and metadata.

#### `void kpm12j_update(Generic_Linear_Sensor_t* s, uint16_t raw_adc)`
Updates displacement for the travel sensor.

#### `float kpm12j_decode(uint8_t high_byte, uint8_t low_byte)`
Decodes mm from the encoded value.

### Usage
```c
#include "KPM12J.h"

void kpm12j_example(void) {
    Generic_Linear_Sensor_t s = {0};
    Generic_Kalman_Settings_t k = {0};
    kpm12j_init(&s, 900, 3100, 0, k);
    kpm12j_update(&s, 2100);
}
```
