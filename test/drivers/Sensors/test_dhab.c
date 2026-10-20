#include "unity.h"
#include "dhab.h"
#include <math.h>
#include <string.h>

static dhab_t sensor;
static dhab_config_t config;

void setUp(void)
{
    config = (dhab_config_t){
        .model = DHAB_S118,
        .adc_full_scale = 65535u,
        .channel = {{1.5f, 0.0f, 0.0f}, {1.5f, 0.0f, 0.0f}}
    };
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, 0u));
}

void tearDown(void) {}

/* Independent voltages at a 5 V sensor supply, before the board divider. */
static uint32_t adc_code(float voltage)
{
    return (uint32_t)roundf(voltage * 65535.0f / 4.95f);
}

static bool update_voltages(float low_v, float high_v, uint32_t now_ms)
{
    return dhab_update(&sensor, adc_code(low_v), adc_code(high_v), 5.0f, 3.3f, now_ms);
}

void test_s118_datasheet_zero_and_positive_and_negative_ten_amps(void)
{
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 1u));
    TEST_ASSERT_EQUAL(DHAB_LOW, sensor.sample.range);
    TEST_ASSERT_INT32_WITHIN(1, 0, sensor.sample.current_ma);
    TEST_ASSERT_TRUE(update_voltages(3.167f, 2.557f, 2u));
    TEST_ASSERT_INT32_WITHIN(1, 10000, sensor.sample.current_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, sensor.sample.channel[DHAB_HIGH].current_a);
    TEST_ASSERT_TRUE(update_voltages(1.833f, 2.443f, 3u));
    TEST_ASSERT_INT32_WITHIN(1, -10000, sensor.sample.current_ma);
}

void test_s118_full_high_range_does_not_overflow_16_bit_centiamps(void)
{
    TEST_ASSERT_TRUE(update_voltages(4.75f, 4.495f, 1u));
    TEST_ASSERT_EQUAL(DHAB_HIGH, sensor.sample.range);
    TEST_ASSERT_INT32_WITHIN(10, 350000, sensor.sample.current_ma);
    TEST_ASSERT_TRUE(update_voltages(0.25f, 0.505f, 2u));
    TEST_ASSERT_INT32_WITHIN(10, -350000, sensor.sample.current_ma);
}

void test_range_selection_uses_unfiltered_low_range_and_hysteresis(void)
{
    TEST_ASSERT_TRUE(update_voltages(4.1675f, 2.6425f, 1u)); /* 25 A */
    TEST_ASSERT_EQUAL(DHAB_LOW, sensor.sample.range);
    TEST_ASSERT_TRUE(update_voltages(4.3676f, 2.6596f, 2u)); /* 28 A */
    TEST_ASSERT_EQUAL(DHAB_HIGH, sensor.sample.range);
    TEST_ASSERT_TRUE(update_voltages(4.1675f, 2.6425f, 3u));
    TEST_ASSERT_EQUAL(DHAB_HIGH, sensor.sample.range);
    TEST_ASSERT_TRUE(update_voltages(4.0341f, 2.6311f, 4u)); /* 23 A */
    TEST_ASSERT_EQUAL(DHAB_LOW, sensor.sample.range);
}

void test_high_range_is_selected_for_low_range_overload_and_recovery(void)
{
    TEST_ASSERT_TRUE(update_voltages(4.75f, 3.07f, 20u)); /* high: 100 A */
    TEST_ASSERT_BITS_HIGH(DHAB_SAMPLE_CLAMP, sensor.sample.channel[DHAB_LOW].flags);
    TEST_ASSERT_INT32_WITHIN(10, 100000, sensor.sample.current_ma);
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 29u));
    TEST_ASSERT_EQUAL(DHAB_HIGH, sensor.sample.range);
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 30u));
    TEST_ASSERT_EQUAL(DHAB_LOW, sensor.sample.range);
}

void test_plausible_but_disagreeing_channels_are_not_silently_selected(void)
{
    TEST_ASSERT_FALSE(update_voltages(2.5f, 2.614f, 1u)); /* 0 A vs 20 A */
    TEST_ASSERT_BITS_HIGH(DHAB_SAMPLE_DISAGREEMENT, sensor.sample.channel[DHAB_LOW].flags);
    TEST_ASSERT_BITS_HIGH(DHAB_SAMPLE_DISAGREEMENT, sensor.sample.channel[DHAB_HIGH].flags);
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 2u));
}

void test_high_overload_and_adc_rails_cannot_publish_plausible_zero(void)
{
    TEST_ASSERT_FALSE(update_voltages(2.5f, 4.75f, 1u));
    TEST_ASSERT_FALSE(sensor.sample.valid);
    TEST_ASSERT_EQUAL(DHAB_NONE, sensor.sample.range);
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 65535u, 5.0f, 3.3f, 2u));
    TEST_ASSERT_BITS_HIGH(DHAB_SAMPLE_ADC_RAIL, sensor.sample.channel[DHAB_HIGH].flags);
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 0u, 5.0f, 3.3f, 3u));
    TEST_ASSERT_FALSE(update_voltages(2.5f, 2.5f, 12u));
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 13u));
}

void test_startup_and_recovery_timers_wrap(void)
{
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, UINT32_MAX));
    TEST_ASSERT_FALSE(update_voltages(2.5f, 2.5f, UINT32_MAX));
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 0u));
    TEST_ASSERT_FALSE(update_voltages(2.5f, 4.75f, UINT32_MAX - 4u));
    TEST_ASSERT_FALSE(update_voltages(2.5f, 2.5f, 4u));
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 5u));
}

void test_actual_supply_scales_offset_and_sensitivity(void)
{
    /* At Uc=4.75 V the datasheet's +10 A voltages scale by 0.95. */
    TEST_ASSERT_TRUE(dhab_update(&sensor, adc_code(3.00865f), adc_code(2.42915f),
                                 4.75f, 3.3f, 1u));
    TEST_ASSERT_INT32_WITHIN(2, 10000, sensor.sample.current_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.02f, 10.0f, sensor.sample.channel[DHAB_HIGH].current_a);
}

void test_12_bit_and_shifted_oversampling_scale_is_explicit(void)
{
    config.adc_full_scale = 4095u;
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, 0u));
    TEST_ASSERT_TRUE(dhab_update(&sensor, 2075u, 2075u, 5.0f, 3.3f, 1u));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.50824f, sensor.sample.channel[DHAB_LOW].sensor_v);
    config.adc_full_scale = 65520u; /* 12-bit sum x16, no right shift. */
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, 0u));
    TEST_ASSERT_TRUE(dhab_update(&sensor, 33200u, 33200u, 5.0f, 3.3f, 1u));
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 2.50824f, sensor.sample.channel[DHAB_LOW].sensor_v);
}

void test_separate_analog_paths_and_explicit_zero_calibration(void)
{
    config.channel[DHAB_LOW] = (dhab_channel_config_t){1.51f, 0.13f, 0.01f};
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, 0u));
    const uint32_t low_raw = (uint32_t)roundf((2.51f - 0.13f) / 1.51f * 65535.0f / 3.3f);
    TEST_ASSERT_TRUE(dhab_update(&sensor, low_raw, adc_code(2.5f), 5.0f, 3.3f, 1u));
    TEST_ASSERT_INT32_WITHIN(1, 0, sensor.sample.current_ma);
}

void test_invalid_inputs_invalidate_previous_measurement(void)
{
    TEST_ASSERT_TRUE(update_voltages(2.5f, 2.5f, 1u));
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 33098u, NAN, 3.3f, 2u));
    TEST_ASSERT_FALSE(sensor.sample.valid);
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 33098u, 5.0f, 0.0f, 3u));
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 65536u, 5.0f, 3.3f, 4u));
    TEST_ASSERT_BITS_HIGH(DHAB_SAMPLE_INPUT, sensor.sample.channel[DHAB_HIGH].flags);
}

void test_init_rejects_bad_config_and_clears_old_state(void)
{
    TEST_ASSERT_FALSE(dhab_init(NULL, &config, 0u));
    TEST_ASSERT_FALSE(dhab_init(&sensor, NULL, 0u));
    config.adc_full_scale = 0u;
    TEST_ASSERT_FALSE(dhab_init(&sensor, &config, 0u));
    config.adc_full_scale = 65535u;
    config.channel[0].divider_gain = NAN;
    TEST_ASSERT_FALSE(dhab_init(&sensor, &config, 0u));
    TEST_ASSERT_FALSE(dhab_update(&sensor, 33098u, 33098u, 5.0f, 3.3f, 1u));
    TEST_ASSERT_FALSE(dhab_update(NULL, 0u, 0u, 5.0f, 3.3f, 1u));
}

void test_s134_transfer_characteristics_are_not_reused_for_s118(void)
{
    config.model = DHAB_S134;
    TEST_ASSERT_TRUE(dhab_init(&sensor, &config, 0u));
    TEST_ASSERT_TRUE(update_voltages(2.9f, 2.6f, 1u));
    TEST_ASSERT_INT32_WITHIN(2, 10000, sensor.sample.current_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, sensor.sample.channel[DHAB_HIGH].current_a);
}
