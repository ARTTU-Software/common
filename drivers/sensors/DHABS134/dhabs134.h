#ifndef INC_DHABS134_H
#define INC_DHABS134_H

#include <stdint.h>
#include "generic_linear_sensor.h"

// For 5k1 series, 10k to GND divider:
// divider_gain = (10000 + 5100) / 10000 = 1.51f
#define DHAB_DIVIDER_GAIN_5K1_10K   1.51f

// Measured Schottky forward voltage drop (on CH50 only)
#define DHAB_SCHOTTKY_DROP_CH50     0.13f  // Actual measured value
#define DHAB_NO_DIODE_CH200         0.0f   // CH200 has no diode

typedef struct {
    float supply_v;                // actual measured Uc (e.g. 4.85f)
    float offset_v;                // Uc/2 ratiometric offset
    float sensitivity_v_per_a;     // sensitivity scaled to actual supply
    float adc_vref;                // actual measured MCU Vref (3.3V nominal)
    float divider_gain;            // voltage divider reconstruction gain (1.51f)
    float diode_drop;              // forward voltage of series diode (0.13V CH50, 0.0V CH200)

    float voltage_raw;             // ADC pin voltage
    float voltage_sensor;          // reconstructed sensor Vout (diode compensated)
    int16_t current;               // filtered current x100 (e.g. 2500 = 25.00 A)

    float alpha;                   // LPF smoothing factor (0–1)

    Generic_Sensor_t generic_sensor;
} DHAB_Channel_t;

typedef struct {
    DHAB_Channel_t ch50;   // Channel 1 (±50 A)
    DHAB_Channel_t ch200;  // Channel 2 (±200 A)
} DHAB_S134_t;

void dhab_s134_init(DHAB_S134_t* s,
                    float supply_v,
                    float adc_vref,
                    float divider_gain,
                    float diode_drop_ch50,
                    float diode_drop_ch200,
                    float alpha,
                    uint8_t port_ch50,
                    uint8_t port_ch200,
                    Generic_Kalman_Settings_t kalman_cfg_50,
                    Generic_Kalman_Settings_t kalman_cfg_200);

void dhab_s134_update_50A(DHAB_S134_t* s);
void dhab_s134_update_200A(DHAB_S134_t* s);
float dhab_s134_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_DHABS134_H */