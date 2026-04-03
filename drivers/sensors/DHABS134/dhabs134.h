#ifndef INC_DHABS134_H
#define INC_DHABS134_H

#include <stdint.h>
#include "generic_linear_sensor.h"

typedef struct {
    float supply_v;                // typically 5.0f
    float offset_v;                // supply/2 = 2.5f
    float sensitivity_v_per_a;     // 0.040 (50A) or 0.010 (200A)
    float adc_vref;                // MCU ADC reference (3.3f)
    float divider_gain;            // optional scaling (1.0 if none)

    float voltage_raw;             // ADC voltage before scaling
    float voltage_sensor;          // reconstructed sensor output voltage
    uint16_t current;                 // filtered current value (A)

    float alpha;                   // smoothing factor (0–1)

    Generic_Sensor_t generic_sensor;
} DHAB_Channel_t;

typedef struct {
    DHAB_Channel_t ch50;   // Channel 1 (50 A)
    DHAB_Channel_t ch200;  // Channel 2 (200 A)
} DHAB_S134_t;

void dhab_s134_init(DHAB_S134_t* s,
                    float supply_v,
                    float adc_vref,
                    float divider_gain,
                    float alpha,
                    uint8_t port_ch50,
                    uint8_t port_ch200,
                    Generic_Kalman_Settings_t kalman_cfg_50,
                    Generic_Kalman_Settings_t kalman_cfg_200);

void dhab_s134_update_50A(DHAB_S134_t* s);
void dhab_s134_update_200A(DHAB_S134_t* s);
float dhab_s134_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_DHABS134_H */
