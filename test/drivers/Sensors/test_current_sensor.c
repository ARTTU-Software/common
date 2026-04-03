#include "unity.h"
#include "dhabs134.h"
#include <string.h>

static DHAB_S134_t test_sensor;

// Default test configuration
const float SUPPLY_V = 5.0f;
const float ADC_VREF = 3.3f;
const float DIVIDER_GAIN = 1.0f;
const float ALPHA_NO_SMOOTHING = 1.0f;

void setUp(void)
{
    memset(&test_sensor, 0, sizeof(DHAB_S134_t));

    dhab_s134_init(&test_sensor,
                   SUPPLY_V,
                   ADC_VREF,
                   DIVIDER_GAIN,
                   ALPHA_NO_SMOOTHING,
                   1, // port_ch50
                   2, // port_ch200
                   (Generic_Kalman_Settings_t){0},
                   (Generic_Kalman_Settings_t){0});
}

void tearDown(void)
{
}

void test_dhab_s134_init_should_initialize_channels_correctly(void)
{
    TEST_ASSERT_EQUAL_FLOAT(0.040f, test_sensor.ch50.sensitivity_v_per_a);
    TEST_ASSERT_EQUAL_FLOAT(0.010f, test_sensor.ch200.sensitivity_v_per_a);
    TEST_ASSERT_EQUAL_UINT8(1, test_sensor.ch50.generic_sensor.port);
    TEST_ASSERT_EQUAL_UINT8(2, test_sensor.ch200.generic_sensor.port);
}

void test_dhab_s134_update_50A_with_zero_current_should_calculate_correctly(void)
{
    // ADC value for 2.5V (zero current)
    test_sensor.ch50.generic_sensor.filtered_value = 3102;
    dhab_s134_update_50A(&test_sensor);
    
    TEST_ASSERT_UINT16_WITHIN(1, 0, test_sensor.ch50.current);
}

void test_dhab_s134_update_50A_with_positive_current_should_calculate_correctly(void)
{
    // ADC value for ~3.5V sensor output (~25A)
    test_sensor.ch50.generic_sensor.filtered_value = 4343;
    dhab_s134_update_50A(&test_sensor);

    // Assert against actual value, not ideal, due to precision loss.
    TEST_ASSERT_EQUAL_UINT16(2499, test_sensor.ch50.current);
}

void test_dhab_s134_update_200A_with_positive_current_should_calculate_correctly(void)
{
    // ADC value for ~3.5V sensor output (~100A)
    test_sensor.ch200.generic_sensor.filtered_value = 4343;
    dhab_s134_update_200A(&test_sensor);
    
    // Assert against actual value, not ideal, due to precision loss.
    TEST_ASSERT_EQUAL_UINT16(9998, test_sensor.ch200.current);
}

void test_dhab_s134_update_should_apply_low_pass_filter(void)
{
    // Re-init with smoothing for this test
    dhab_s134_init(&test_sensor, SUPPLY_V, ADC_VREF, DIVIDER_GAIN, 0.5f, 1, 2, 
                   (Generic_Kalman_Settings_t){0}, (Generic_Kalman_Settings_t){0});

    // Previous state: 10A (scaled to 1000)
    test_sensor.ch50.current = 1000;

    // New reading: ~20A raw (corresponding ADC value for 3.3V)
    test_sensor.ch50.generic_sensor.filtered_value = 4095;
    
    dhab_s134_update_50A(&test_sensor);

    // Expected: 0.5*20A (raw) + 0.5*10A (previous) = 15A. Scaled to 1500.
    TEST_ASSERT_UINT16_WITHIN(1, 1500, test_sensor.ch50.current);
}

void test_dhab_s134_decode_should_decode_bytes(void)
{
    // 12345 -> 123.45 A
    float actual = dhab_s134_decode(0x30, 0x39);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 123.45f, actual);
}