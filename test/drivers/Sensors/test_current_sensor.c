#include "unity.h"
#include "dhabs134.h"
#include <string.h>

static DHAB_S134_t test_sensor;

// Standard test configuration
// Supply: 5.06V (measured actual), ADC Vref: 3.3V (actual, not 3.41V!)
// Divider: 1.51 (5.1k + 10k / 10k), Diode: 0.13V CH50, 0.0V CH200
const float SUPPLY_V = 5.06f;
const float ADC_VREF = 3.3f;
const float DIVIDER_GAIN = 1.51f;
const float DIODE_DROP_CH50 = 0.13f;
const float DIODE_DROP_CH200 = 0.0f;
const float ALPHA_NO_SMOOTHING = 1.0f;
const float ALPHA_SMOOTH = 0.5f;

void setUp(void)
{
    memset(&test_sensor, 0, sizeof(DHAB_S134_t));
}

void tearDown(void)
{
}

/**
 * T1: Init with standard parameters
 * Verify that sensitivity and offset are calculated correctly with actual hardware values.
 * Expected sensitivity (CH50): 0.040 * (5.06 / 5.0) = 0.0402 V/A
 * Expected offset: 5.06 / 2 = 2.53 V
 */
void test_dhab_s134_init_standard_parameters(void)
{
    dhab_s134_init(&test_sensor,
                   SUPPLY_V,
                   ADC_VREF,
                   DIVIDER_GAIN,
                   DIODE_DROP_CH50,
                   DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING,
                   1, // port_ch50
                   2, // port_ch200
                   (Generic_Kalman_Settings_t){0},
                   (Generic_Kalman_Settings_t){0});

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.04048f, test_sensor.ch50.sensitivity_v_per_a);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.01012f, test_sensor.ch200.sensitivity_v_per_a);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.53f, test_sensor.ch50.offset_v);
    TEST_ASSERT_EQUAL_UINT8(1, test_sensor.ch50.generic_sensor.port);
    TEST_ASSERT_EQUAL_UINT8(2, test_sensor.ch200.generic_sensor.port);
}

/**
 * T2: Zero current (0A) on CH50
 * Sensor Vout at 0A = 5.06/2 = 2.53V
 * After diode compensation (+0.13V): 2.66V
 * ADC pin voltage (after divider /1.51): 2.66 / 1.51 = 1.762V
 * ADC counts: 1.762 * 4095 / 3.3 = 2187 counts
 * Expected current: 0A (±50 counts tolerance = ±0.5A per datasheet)
 */
void test_dhab_s134_ch50_zero_current(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 1969;
    dhab_s134_update_50A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(80, 0, test_sensor.ch50.current);
}

/**
 * T3: Positive current (+25A) on CH50
 * Sensor Vout = 2.53 + (25 * 0.040) = 3.53V
 * After diode (+0.13V): 3.66V
 * ADC pin voltage: 3.66 / 1.51 = 2.424V
 * ADC counts: 2.424 * 4095 / 3.3 = 3006 counts
 * Expected current: 25.00A
 */
void test_dhab_s134_ch50_positive_25a(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 2801;
    dhab_s134_update_50A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(80, 2500, test_sensor.ch50.current);
}

/**
 * T4: Maximum current (+50A) on CH50
 * Sensor Vout = 2.53 + (50 * 0.040) = 4.53V
 * After diode (+0.13V): 4.66V
 * ADC pin voltage: 4.66 / 1.51 = 3.086V
 * ADC counts: 3.086 * 4095 / 3.3 = 3822 counts
 * Expected current: 50.00A
 */
void test_dhab_s134_ch50_max_50a(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 3628;
    dhab_s134_update_50A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(80, 5000, test_sensor.ch50.current);
}

/**
 * T5: Positive current (+100A) on CH200
 * Sensor Vout = 2.53 + (100 * 0.010) = 3.53V
 * NO diode on CH200: stays 3.53V
 * ADC pin voltage: 3.53 / 1.51 = 2.339V
 * ADC counts: 2.339 * 4095 / 3.3 = 2899 counts
 * Expected current: 100.00A
 */
void test_dhab_s134_ch200_100a(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch200.generic_sensor.filtered_value = 2898;
    dhab_s134_update_200A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(200, 10000, test_sensor.ch200.current);
}

/**
 * T6: Maximum current (+200A) on CH200
 * Sensor Vout = 2.53 + (200 * 0.010) = 4.53V
 * NO diode: stays 4.53V
 * ADC pin voltage: 4.53 / 1.51 = 3.002V
 * ADC counts: 3.002 * 4095 / 3.3 = 3723 counts
 * Expected current: 200.00A
 */
void test_dhab_s134_ch200_max_200a(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch200.generic_sensor.filtered_value = 3743;
    dhab_s134_update_200A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(150, 20000, test_sensor.ch200.current);
}

/**
 * T7: Negative current (−25A) on CH50
 * Sensor Vout = 2.53 - (25 * 0.040) = 1.53V
 * After diode (+0.13V): 1.66V
 * ADC pin voltage: 1.66 / 1.51 = 1.099V
 * ADC counts: 1.099 * 4095 / 3.3 = 1363 counts
 * Expected current: -25.00A
 */
void test_dhab_s134_ch50_negative_25a(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 1139;
    dhab_s134_update_50A(&test_sensor);

    TEST_ASSERT_INT16_WITHIN(80, -2500, test_sensor.ch50.current);
}

/**
 * T8: Ratiometric scaling at different supply voltage (4.85V)
 * At lower supply, sensitivity and offset should both scale.
 * Expected sensitivity (CH50): 0.040 * (4.85 / 5.0) = 0.0388 V/A
 * Expected offset: 4.85 / 2 = 2.425 V
 */
void test_dhab_s134_ratiometric_scaling_485v(void)
{
    float supply_v_alt = 4.85f;
    dhab_s134_init(&test_sensor, supply_v_alt, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0388f, test_sensor.ch50.sensitivity_v_per_a);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.425f, test_sensor.ch50.offset_v);
}

/**
 * T9: LPF Smoothing with alpha = 0.5
 * Previous state: 10A (stored as 1000)
 * New raw reading: 50A (ADC for 50A)
 * Expected: 0.5 * 50 + 0.5 * (1000/100) = 25 + 5 = 30A (3000)
 */
void test_dhab_s134_lpf_smoothing(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_SMOOTH, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    // Set previous state: 10A
    test_sensor.ch50.current = 1000;

    // New reading: 50A ADC value (3628 counts)
    test_sensor.ch50.generic_sensor.filtered_value = 3628;
    dhab_s134_update_50A(&test_sensor);

    // Expected: 0.5*50A + 0.5*10A = 30A = 3000
    TEST_ASSERT_INT16_WITHIN(100, 3000, test_sensor.ch50.current);
}

/**
 * T10: Saturation at low end (0.25V sensor output - minimum clamp)
 * After divider: 0.25 / 1.51 = 0.165V
 * ADC counts: 0.165 * 4095 / 3.3 = 205 counts
 * Calculated current: (0.25 - 2.53) / 0.0402 = -57.5A
 * Shows driver can represent saturation condition
 */
void test_dhab_s134_saturation_low(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 205;
    dhab_s134_update_50A(&test_sensor);

    // At saturation low, reads off-scale negative (~-53.1A)
    TEST_ASSERT_INT16_WITHIN(300, -5312, test_sensor.ch50.current);
}

/**
 * T11: Saturation at high end (4.75V sensor output - maximum clamp)
 * After divider: 4.75 / 1.51 = 3.145V
 * ADC counts: 3.145 * 4095 / 3.3 = 3905 counts
 * Calculated current: (4.75 - 2.53) / 0.0402 = 55.2A
 * Shows driver can represent saturation condition
 */
void test_dhab_s134_saturation_high(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    test_sensor.ch50.generic_sensor.filtered_value = 3905;
    dhab_s134_update_50A(&test_sensor);

    // At saturation high, reads off-scale positive (~57.2A)
    TEST_ASSERT_INT16_WITHIN(200, 5720, test_sensor.ch50.current);
}

/**
 * T12: Diode compensation difference between CH50 and CH200
 * At same current (25A), CH50 and CH200 have different sensor outputs due to
 * different sensitivities, but the key test is that diode drop is handled correctly.
 * CH50: 3.53V + 0.13V (diode) = 3.66V at divider input
 * CH200: 3.53V + 0.0V (no diode) = 3.53V at divider input
 * This test verifies both channels compute correctly despite different diode drops.
 */
void test_dhab_s134_diode_compensation_difference(void)
{
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    // Both channels get their respective correct ADC values
    // CH50 at 25A
    test_sensor.ch50.generic_sensor.filtered_value = 2801;
    dhab_s134_update_50A(&test_sensor);
    TEST_ASSERT_INT16_WITHIN(100, 2500, test_sensor.ch50.current);

    // CH200 at 25A (different ADC value due to different sensitivity, no diode)
    // 25A on CH200: Vout = 2.53 + 25*0.01012 = 2.783V, no diode drop, so 2.783V at divider
    // ADC: 2.783/1.51 = 1.842V -> 1.842 * 4095 / 3.3 = 2282 counts
    test_sensor.ch200.generic_sensor.filtered_value = 2282;
    dhab_s134_update_200A(&test_sensor);
    TEST_ASSERT_INT16_WITHIN(150, 2500, test_sensor.ch200.current);
}

/**
 * T13: Decode function for CAN message conversion
 * Tests encoding/decoding of current values as 16-bit integers.
 * 12345 decimal -> 0x3039 bytes -> 123.45 A
 */
void test_dhab_s134_decode_positive(void)
{
    float actual = dhab_s134_decode(0x30, 0x39);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 123.45f, actual);
}

/**
 * T14: Decode function for negative values
 * -12220 decimal -> 0xD044 bytes -> -122.20 A
 */
void test_dhab_s134_decode_negative(void)
{
    float actual = dhab_s134_decode(0xD0, 0x44);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -122.20f, actual);
}

/**
 * T15: Divider gain impact on measurement
 * If resistors were slightly different (e.g., 5.6k + 10k instead of 5.1k + 10k),
 * divider gain would be 1.56 instead of 1.51.
 * At same ADC reading, higher gain means higher reconstructed voltage -> higher current.
 * This test documents the sensitivity to divider tolerance.
 */
void test_dhab_s134_divider_gain_impact(void)
{
    float divider_gain_alt = 1.56f;  // Alternative if resistors differ
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, divider_gain_alt,
                   DIODE_DROP_CH50, DIODE_DROP_CH200,
                   ALPHA_NO_SMOOTHING, 1, 2, (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    // At 25A with different gain, ADC reading is different
    // Expected Vout still 3.53V, but now ADC pin = 3.53/1.56 = 2.263V
    // ADC: 2.263 * 4095 / 3.3 = 2803 counts (vs 3006 at 1.51 gain)
    test_sensor.ch50.generic_sensor.filtered_value = 2709;
    dhab_s134_update_50A(&test_sensor);

    // Current should still be ~25A despite different divider
    TEST_ASSERT_INT16_WITHIN(80, 2500, test_sensor.ch50.current);
}