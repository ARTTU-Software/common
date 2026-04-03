#include "dhabs134.h"

static void init_channel(DHAB_Channel_t* ch,
                         float supply_v,
                         float adc_vref,
                         float divider_gain,
                         float sensitivity,
                         float alpha,
                         uint8_t port,
                         Generic_Kalman_Settings_t kalman){
    ch->supply_v = supply_v;
    ch->offset_v = supply_v * 0.5f;     // VO = Uc/2
    ch->adc_vref = adc_vref;
    ch->divider_gain = divider_gain;
    ch->sensitivity_v_per_a = sensitivity;
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
                    float alpha,
                    uint8_t port_ch50,
                    uint8_t port_ch200,
                    Generic_Kalman_Settings_t kalman_cfg_50,
                    Generic_Kalman_Settings_t kalman_cfg_200){
    // Channel 1 = 50 A, 40 mV/A
    init_channel(&s->ch50,
                 supply_v,
                 adc_vref,
                 divider_gain,
                 0.040f,               // 40 mV/A
                 alpha,
                 port_ch50,
                 kalman_cfg_50);

    // Channel 2 = 200 A, 10 mV/A
    init_channel(&s->ch200,
                 supply_v,
                 adc_vref,
                 divider_gain,
                 0.010f,               // 10 mV/A
                 alpha,
                 port_ch200,
                 kalman_cfg_200);
}

static uint16_t update_channel(DHAB_Channel_t* ch){
    // Convert ADC to MCU volts
    ch->voltage_raw = ch->generic_sensor.filtered_value * (ch->adc_vref / 4095.0f);

    // Compensate voltage divider (if any)
    ch->voltage_sensor = ch->voltage_raw * ch->divider_gain;

    // DHAB formula: IP = (Vout - VO) / G
    float raw_current = (ch->voltage_sensor - ch->offset_v) /
                         ch->sensitivity_v_per_a;

    // LPF smoothing
    float filtered_current = ch->alpha * raw_current +
                  (1.0f - ch->alpha) * ch->current / 100.0f;

    ch->current = (uint16_t)(filtered_current * 100.0f);

    return ch->current;
}

void dhab_s134_update_50A(DHAB_S134_t* s){
    update_channel(&s->ch50);
}

void dhab_s134_update_200A(DHAB_S134_t* s){
    update_channel(&s->ch200);
}

float dhab_s134_decode(uint8_t high_byte, uint8_t low_byte){
    uint16_t value = (high_byte << 8) | low_byte;
    return value / 100.0f;
}