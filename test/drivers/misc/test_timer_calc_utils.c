#include "timer_calc_utils.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_timer_calc_pwm_valid_inputs(void)
{
    uint16_t psc, arr, ccr;
    // Freq = 1kHz, Duty = 50%, Bus = 84MHz
    // period_ms = 1000 / 1000 = 1ms
    // total_ticks = 1 * 84 * 1000 = 84000
    // psc_val = ceil(84000 / 65536) = 2
    // psc = 1
    // arr_val = 84000 / 2 = 42000
    // arr = 41999
    // ccr = 41999 * 50 / 100 = 20999
    bool result = timer_calc_pwm(1000, 50, 84, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(1, psc);
    TEST_ASSERT_EQUAL_UINT16(41999, arr);
    TEST_ASSERT_EQUAL_UINT16(20999, ccr);
}

void test_timer_calc_pwm_zero_frequency(void)
{
    uint16_t psc, arr, ccr;
    bool result = timer_calc_pwm(0, 50, 84, &psc, &arr, &ccr);
    TEST_ASSERT_FALSE(result);
}

void test_timer_calc_pwm_invalid_duty_cycle(void)
{
    uint16_t psc, arr, ccr;
    bool result = timer_calc_pwm(1000, 101, 84, &psc, &arr, &ccr);
    TEST_ASSERT_FALSE(result);
}

void test_timer_calc_pwm_null_pointers(void)
{
    uint16_t psc, arr, ccr;
    TEST_ASSERT_FALSE(timer_calc_pwm(1000, 50, 84, NULL, &arr, &ccr));
    TEST_ASSERT_FALSE(timer_calc_pwm(1000, 50, 84, &psc, NULL, &ccr));
    TEST_ASSERT_FALSE(timer_calc_pwm(1000, 50, 84, &psc, &arr, NULL));
}

void test_timer_calc_pwm_duty_cycle_zero(void)
{
    uint16_t psc, arr, ccr;
    bool result = timer_calc_pwm(1000, 0, 84, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0, ccr);
}

void test_timer_calc_pwm_duty_cycle_hundred(void)
{
    uint16_t psc, arr, ccr;
    bool result = timer_calc_pwm(1000, 100, 84, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(arr, ccr);
}

void test_timer_calc_pwm_low_frequency(void)
{
    uint16_t psc, arr, ccr;
    // Freq = 10Hz, Duty = 25%, Bus = 42MHz
    // total_ticks = (42 * 1000000) / 10 = 4200000
    // psc_val = ceil(4200000 / 65536) = 65
    // psc = 64
    // arr_val = 4200000 / 65 = 64615
    // arr = 64614
    // ccr = 64614 * 25 / 100 = 16153
    bool result = timer_calc_pwm(10, 25, 42, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(64, psc);
    TEST_ASSERT_EQUAL_UINT16(64614, arr);
    TEST_ASSERT_EQUAL_UINT16(16153, ccr);
}

void test_timer_calc_pwm_high_frequency_10khz(void)
{
    uint16_t psc, arr, ccr;
    // Freq = 10kHz, Duty = 50%, Bus = 84MHz
    // total_ticks = 84,000,000 / 10,000 = 8400 ticks (fits directly in 16-bit ARR)
    // psc = 0
    // arr = 8399
    // ccr = 8399 * 50 / 100 = 4199
    bool result = timer_calc_pwm(10000, 50, 84, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0, psc);
    TEST_ASSERT_EQUAL_UINT16(8399, arr);
    TEST_ASSERT_EQUAL_UINT16(4199, ccr);
}

void test_timer_calc_pwm_high_frequency_20khz(void)
{
    uint16_t psc, arr, ccr;
    // Freq = 20kHz, Duty = 75%, Bus = 170MHz
    // total_ticks = 170,000,000 / 20,000 = 8500 ticks
    // psc = 0
    // arr = 8499
    // ccr = 8499 * 75 / 100 = 6374
    bool result = timer_calc_pwm(20000, 75, 170, &psc, &arr, &ccr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0, psc);
    TEST_ASSERT_EQUAL_UINT16(8499, arr);
    TEST_ASSERT_EQUAL_UINT16(6374, ccr);
}

void test_timer_calculate_freq_psc_arr(void)
{
    uint16_t psc, arr;
    // 5 kHz on 170 MHz bus -> total_ticks = 170,000,000 / 5,000 = 34,000
    bool result = timer_calculate_freq_psc_arr(5000, 170, &psc, &arr);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0, psc);
    TEST_ASSERT_EQUAL_UINT16(33999, arr);

    // Invalid parameters
    TEST_ASSERT_FALSE(timer_calculate_freq_psc_arr(0, 170, &psc, &arr));
    TEST_ASSERT_FALSE(timer_calculate_freq_psc_arr(5000, 0, &psc, &arr));
    TEST_ASSERT_FALSE(timer_calculate_freq_psc_arr(5000, 170, NULL, &arr));
}