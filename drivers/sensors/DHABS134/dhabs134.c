#include "dhabs134.h"

static void init_channel(DHAB_Channel_t* ch,
                         float supply_v,
                         float adc_vref,
                         float divider_gain,
                         float diode_drop,
                         float nominal_sensitivity,
                         float expected_current, // NEW
                         float measured_current, // NEW
                         float alpha,
                         uint8_t port,
                         Generic_Kalman_Settings_t kalman){

    ch->supply_v = supply_v;
    ch->offset_v = supply_v * 0.5f;            
    ch->adc_vref = adc_vref;
    ch->divider_gain = divider_gain;
    ch->diode_drop = diode_drop;

    // Scale sensitivity ratiometrically to actual supply voltage
    ch->sensitivity_v_per_a = nominal_sensitivity * (supply_v / 5.0f);

    // NEW: Auto-calculate calibration multiplier (with safety catch for div-by-zero)
    if (measured_current != 0.0f && expected_current != 0.0f) {
        ch->calibration_multiplier = expected_current / measured_current;
    } else {
        ch->calibration_multiplier = 1.0f; // Default to no calibration
    }

    ch->alpha = alpha;

    ch->voltage_raw = 0.0f;
    ch->voltage_sensor = 0.0f;
    ch->current = 0;

    ch->generic_sensor.port = port;
    ch->generic_sensor.kalman_settings = kalman;
}

void dhab_s134_init(DHAB_S134_t* s,
                    float supply_v,
                    float adc_vref,
                    float divider_gain,
                    float diode_drop_ch50,
                    float diode_drop_ch200,
                    float expected_current_ch50, 
                    float measured_current_ch50, 
                    float expected_current_ch200,
                    float measured_current_ch200,
                    float alpha,
                    uint8_t port_ch50,
                    uint8_t port_ch200,
                    Generic_Kalman_Settings_t kalman_cfg_50,
                    Generic_Kalman_Settings_t kalman_cfg_200){

    // Channel 1 = ±50 A, 40 mV/A nominal
    init_channel(&s->ch50,
                 supply_v,
                 adc_vref,
                 divider_gain,
                 diode_drop_ch50,
                 0.040f,
                 expected_current_ch50,   // Pass calib values
                 measured_current_ch50,   // Pass calib values
                 alpha,
                 port_ch50,
                 kalman_cfg_50);

    // Channel 2 = ±200 A, 10 mV/A nominal
    init_channel(&s->ch200,
                 supply_v,
                 adc_vref,
                 divider_gain,
                 diode_drop_ch200,
                 0.010f,
                 expected_current_ch200,  // Pass calib values
                 measured_current_ch200,  // Pass calib values
                 alpha,
                 port_ch200,
                 kalman_cfg_200);
}

static int16_t update_channel(DHAB_Channel_t* ch){
    // Convert ADC counts to MCU pin voltage
    ch->voltage_raw = ch->generic_sensor.filtered_value * (ch->adc_vref / 4095.0f);

    // Reconstruct sensor Vout: undo divider, then undo diode drop
    ch->voltage_sensor = (ch->voltage_raw * ch->divider_gain) + ch->diode_drop;

    // Calculate uncalibrated current
    float raw_current = (ch->voltage_sensor - ch->offset_v) / ch->sensitivity_v_per_a;

    // NEW: Apply calibration multiplier to fix hardware tolerances
    raw_current *= ch->calibration_multiplier;

    // LPF smoothing
    float filtered_current = (ch->alpha * raw_current) +
                             ((1.0f - ch->alpha) * (ch->current / 100.0f));

    // Store as x100 integer (e.g. 25.14 A -> 2514)
    ch->current = (int16_t)(filtered_current * 100.0f);

    return ch->current;
}

void dhab_s134_update_50A(DHAB_S134_t* s){
    update_channel(&s->ch50);
}

void dhab_s134_update_200A(DHAB_S134_t* s){
    update_channel(&s->ch200);
}

float dhab_s134_decode(uint8_t high_byte, uint8_t low_byte){
    int16_t value = (int16_t)((high_byte << 8) | low_byte);
    return value / 100.0f;
}