#include "ge1935.h"

// R‑vs‑T lookup table from GE1935 datasheet
typedef struct { float t_c; float r_ohm; } NtcPoint;
static const NtcPoint ge1935_lut[] = {
    { -40.0f, 332776.0f },
    { -20.0f, 96481.0f },
    {   0.0f, 32566.0f },
    {  20.0f, 12486.0f },
    {  25.0f, 10000.0f },
    {  40.0f, 5331.0f },
    {  60.0f, 2490.0f },
    {  85.0f, 1071.0f },
    { 100.0f, 678.1f },
    { 120.0f, 387.3f }
};
static const int LUT_SIZE = sizeof(ge1935_lut)/sizeof(ge1935_lut[0]);

void ge1935_init(GE1935_t* s,
                 uint8_t port,
                 float v_ref,
                 float r_fixed,
                 Generic_Kalman_Settings_t k)
{
    s->generic_sensor.port = port;
    s->generic_sensor.kalman_settings = k;
    s->v_ref = v_ref;
    s->r_fixed = r_fixed;
    s->temperature = 0;
}

static float resistance_to_temp(uint16_t r_adc_scaled) {
    float t = 0.0f;

    // linear interpolation between LUT points
    for (int i = 0; i < LUT_SIZE - 1; i++) {
        float r_hi = ge1935_lut[i].r_ohm;
        float r_lo = ge1935_lut[i+1].r_ohm;
        if (r_adc_scaled <= r_hi && r_adc_scaled >= r_lo) {
            float t_hi = ge1935_lut[i].t_c;
            float t_lo = ge1935_lut[i+1].t_c;
            t = t_lo + (r_lo - r_adc_scaled) * (t_hi - t_lo) / (r_lo - r_hi);
            return t;
        }
    }

    // out-of-range: clamp
    if (r_adc_scaled > ge1935_lut[0].r_ohm) return ge1935_lut[0].t_c;
    if (r_adc_scaled < ge1935_lut[LUT_SIZE-1].r_ohm) return ge1935_lut[LUT_SIZE-1].t_c;
    return t;
}

void ge1935_update(GE1935_t* s, uint16_t filtered_adc) {
    s->generic_sensor.filtered_value = filtered_adc;

    // convert ADC to voltage
    float v = ((float)filtered_adc) * (s->v_ref / 4095.0f);

    // compute thermistor resistance
    float r_therm = s->r_fixed * ((s->v_ref / v) - 1.0f);

    // convert resistance → temperature (°C)
    float temp_c = resistance_to_temp(r_therm);

    // store as uint16_t scaled value (°C * 1000)
    if (temp_c < -40.0f) temp_c = -40.0f;
    if (temp_c > 120.0f) temp_c = 120.0f;
    s->temperature = (uint16_t)(temp_c * 1000.0f);
}

float ge1935_decode(uint8_t high_byte, uint8_t low_byte) {
    uint16_t value = (high_byte << 8) | low_byte;
    return value / 1000.0f;
}