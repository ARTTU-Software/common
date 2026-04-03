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
    // period_ms = 1000 / 10 = 100ms
    // total_ticks = 100 * 42 * 1000 = 4200000
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