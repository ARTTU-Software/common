#include "moving_avg.h"
#include "unity.h"
#include <string.h>

#define WINDOW_SIZE 4

static Moving_Avg_t filter;
static uint32_t storage[WINDOW_SIZE];

void setUp(void)
{
    memset(storage, 0, sizeof(storage));
    moving_avg_init(&filter, storage, WINDOW_SIZE);
}

void tearDown(void)
{
}

void test_moving_avg_init_null_checks(void)
{
    Moving_Avg_t f;
    uint32_t mem[4];

    TEST_ASSERT_FALSE(moving_avg_init(NULL, mem, 4));
    TEST_ASSERT_FALSE(moving_avg_init(&f, NULL, 4));
    TEST_ASSERT_FALSE(moving_avg_init(&f, mem, 0));
    TEST_ASSERT_TRUE(moving_avg_init(&f, mem, 4));
}

void test_moving_avg_filling_window(void)
{
    // Window = 4
    // Sample 1: 100 -> avg = 100 / 1 = 100
    TEST_ASSERT_EQUAL_UINT32(100, moving_avg_update(&filter, 100));
    TEST_ASSERT_FALSE(moving_avg_is_full(&filter));
    TEST_ASSERT_EQUAL_UINT32(100, moving_avg_get(&filter));

    // Sample 2: 200 -> avg = (100 + 200) / 2 = 150
    TEST_ASSERT_EQUAL_UINT32(150, moving_avg_update(&filter, 200));
    TEST_ASSERT_FALSE(moving_avg_is_full(&filter));

    // Sample 3: 300 -> avg = (100 + 200 + 300) / 3 = 200
    TEST_ASSERT_EQUAL_UINT32(200, moving_avg_update(&filter, 300));
    TEST_ASSERT_FALSE(moving_avg_is_full(&filter));

    // Sample 4: 400 -> avg = (100 + 200 + 300 + 400) / 4 = 250
    TEST_ASSERT_EQUAL_UINT32(250, moving_avg_update(&filter, 400));
    TEST_ASSERT_TRUE(moving_avg_is_full(&filter));
    TEST_ASSERT_EQUAL_UINT32(250, moving_avg_get(&filter));
}

void test_moving_avg_sliding_window_eviction(void)
{
    // Fill with [10, 10, 10, 10] -> avg = 10
    moving_avg_update(&filter, 10);
    moving_avg_update(&filter, 10);
    moving_avg_update(&filter, 10);
    uint32_t avg = moving_avg_update(&filter, 10);
    TEST_ASSERT_EQUAL_UINT32(10, avg);
    TEST_ASSERT_TRUE(moving_avg_is_full(&filter));

    // Replace first 10 with 50 -> buffer is [50, 10, 10, 10] -> sum = 80 -> avg = 20
    TEST_ASSERT_EQUAL_UINT32(20, moving_avg_update(&filter, 50));

    // Replace second 10 with 50 -> buffer is [50, 50, 10, 10] -> sum = 120 -> avg = 30
    TEST_ASSERT_EQUAL_UINT32(30, moving_avg_update(&filter, 50));

    // Replace third 10 with 50 -> buffer is [50, 50, 50, 10] -> sum = 160 -> avg = 40
    TEST_ASSERT_EQUAL_UINT32(40, moving_avg_update(&filter, 50));

    // Replace fourth 10 with 50 -> buffer is [50, 50, 50, 50] -> sum = 200 -> avg = 50
    TEST_ASSERT_EQUAL_UINT32(50, moving_avg_update(&filter, 50));
}

void test_moving_avg_reset(void)
{
    moving_avg_update(&filter, 500);
    moving_avg_update(&filter, 500);
    TEST_ASSERT_EQUAL_UINT32(500, moving_avg_get(&filter));

    moving_avg_reset(&filter);
    TEST_ASSERT_EQUAL_UINT32(0, moving_avg_get(&filter));
    TEST_ASSERT_FALSE(moving_avg_is_full(&filter));
}
