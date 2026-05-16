---
title: Common - DHABS134
description: Dual-channel current sensor driver.
---

# DHABS134

Dual-channel current sensor driver (50A and 200A) with ratiometric scaling and low-pass filtering. Output current is stored as an x100 integer (for example 2514 means 25.14 A).

## Functions
#### `void dhab_s134_init(DHAB_S134_t* s, float supply_v, float adc_vref, float divider_gain, float diode_drop_ch50, float diode_drop_ch200, float alpha, uint8_t port_ch50, uint8_t port_ch200, Generic_Kalman_Settings_t kalman_cfg_50, Generic_Kalman_Settings_t kalman_cfg_200)`
Initializes both channels and calibration parameters.

#### `void dhab_s134_update_50A(DHAB_S134_t* s)`
Updates the 50A channel current estimate.

#### `void dhab_s134_update_200A(DHAB_S134_t* s)`
Updates the 200A channel current estimate.

#### `float dhab_s134_decode(uint8_t high_byte, uint8_t low_byte)`
Decodes the x100 current into float amps.

### Usage
```c
#include "dhabs134.h"

void dhabs_example(void) {
    DHAB_S134_t sensor = {0};
    Generic_Kalman_Settings_t k = {0};
    dhab_s134_init(&sensor, 4.85f, 3.3f, DHAB_DIVIDER_GAIN_5K1_10K,
                   DHAB_SCHOTTKY_DROP_CH50, DHAB_NO_DIODE_CH200,
                   0.2f, 0, 1, k, k);
    dhab_s134_update_50A(&sensor);
}
```
