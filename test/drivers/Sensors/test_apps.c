#include "generic_sensor.h"
#include "unity.h"
#include "apps.h"
#include "generic_linear_sensor.h"
#include <string.h> // For memset

void setUp(void)
{
}

void tearDown(void)
{
}

void test_APPS_init_should_correctly_initialize_struct(void)
{
    // 1. Setup
    Generic_Linear_Sensor_t sensor;
    memset(&sensor, 0, sizeof(Generic_Linear_Sensor_t)); // Clear the struct

    Generic_Kalman_Settings_t kalman_settings = {
        .state = {1.0f},
        .covariance = {1.0f},
        .R = {0.1f},
        .Ht = {1.0f},
        .chi2 = 0.0f,
        .Phi = {1.0f},
        .G = {1.0f},
        .Q = {0.01f}
    };
    uint16_t min_adc = 100;
    uint16_t max_adc = 4000;
    uint8_t port = 3;

    // 2. Action
    APPS_init(&sensor, min_adc, max_adc, port, kalman_settings);

    // 3. Assertion
    TEST_ASSERT_EQUAL_FLOAT(max_adc, sensor.max_adc);
    TEST_ASSERT_EQUAL_FLOAT(min_adc, sensor.min_adc);
    TEST_ASSERT_EQUAL_UINT8(port, sensor.generic_sensor.port);
    TEST_ASSERT_EQUAL_FLOAT(kalman_settings.R[0], sensor.generic_sensor.kalman_settings.R[0]);
    TEST_ASSERT_EQUAL_FLOAT(kalman_settings.Q[0], sensor.generic_sensor.kalman_settings.Q[0]);
}

void test_APPS_update_should_calculate_voltage_and_displacement(void)
{
    // 1. Setup
    Generic_Linear_Sensor_t sensor;
    Generic_Kalman_Settings_t kalman_settings = {0}; // Kalman settings are not used in this test
    APPS_init(&sensor, 410, 3686, 1, kalman_settings); // Min/Max for 10-90% of 12-bit ADC

    uint16_t raw = 2048; // A value in the middle (approx 50%)

    // 2. Action
    APPS_update(&sensor, raw);

    // 3. Assertion
    TEST_ASSERT_EQUAL_UINT16(12.5, sensor.generic_sensor.raw_value);

    // Voltage = 2048 * (3.3 / 4095) = 1.651...
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.651f, sensor.voltage);

    // norm = (2048 - 410) / (3686 - 410) = 1638 / 3276 = 0.5
    // displacement = 0.5 * 25.0 = 12.5 mm
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 12.5f, sensor.displacement);
}

void test_APPS_update_should_clamp_at_lower_bound(void)
{
    // 1. Setup
    Generic_Linear_Sensor_t sensor;
    APPS_init(&sensor, 410, 3686, 1, (Generic_Kalman_Settings_t){0});
    
    // 2. Action
    APPS_update(&sensor, 300); // Value below min_adc

    // 3. Assertion
    // norm should be clamped to 0
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, sensor.displacement);
}

void test_APPS_update_should_clamp_at_upper_bound(void)
{
    // 1. Setup
    Generic_Linear_Sensor_t sensor;
    APPS_init(&sensor, 410, 3686, 1, (Generic_Kalman_Settings_t){0});

    // 2. Action
    APPS_update(&sensor, 4000); // Value above max_adc

    // 3. Assertion
    // norm should be clamped to 1.0, displacement = 1.0 * 25.0 = 25.0 mm
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.0f, sensor.displacement);
}

// This test checks behavior when min and max are the same to avoid division by zero.
// The current implementation has a bug here. This test exposes it.
void test_APPS_update_with_zero_range_should_behave_predictably(void)
{
    // 1. Setup
    Generic_Linear_Sensor_t sensor;
    APPS_init(&sensor, 1000, 1000, 1, (Generic_Kalman_Settings_t){0});

    // 2. Action
    APPS_update(&sensor, 1000);

    // 3. Assertion
    // norm = (1000 - 1000) / (1000 - 1000) = 0 / 0 -> NaN
    // The code doesn't handle this. The clamping `if (norm < 0.0f)` is false for NaN.
    // A robust implementation would check for (max_adc - min_adc) == 0.
    // We expect a predictable output, e.g. 0 displacement.
    // The actual output might be unpredictable garbage due to NaN propagation.
    // Let's assert for 0, which is the safe/desired outcome.
    TEST_ASSERT_EQUAL_UINT16(0, sensor.displacement);
}


void test_APPS_decode_should_correctly_decode_bytes(void)
{
    // Test case 1: 12.345mm
    uint8_t high_byte = 0x30; // 12345 = 0x3039
    uint8_t low_byte = 0x39;
    float expected_value = 12.345f;
    float actual_value = APPS_decode(high_byte, low_byte);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected_value, actual_value);

    // Test case 2: 0mm
    high_byte = 0;
    low_byte = 0;
    expected_value = 0.0f;
    actual_value = APPS_decode(high_byte, low_byte);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected_value, actual_value);
    
    // Test case 3: max value 65.535mm
    high_byte = 0xFF;
    low_byte = 0xFF;
    expected_value = 65.535f;
    actual_value = APPS_decode(high_byte, low_byte);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected_value, actual_value);
}