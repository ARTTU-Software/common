#include "generic_ring_buffer.h"
#include "unity.h"
#include <string.h>

#define TEST_BUF_SIZE 16

static Ring_Buffer_t rb;
static uint8_t storage[TEST_BUF_SIZE];

void setUp(void)
{
    memset(storage, 0, sizeof(storage));
    ring_buffer_init(&rb, storage, TEST_BUF_SIZE);
}

void tearDown(void)
{
}

void test_ring_buffer_init_null_checks(void)
{
    Ring_Buffer_t test_rb;
    uint8_t mem[16];

    TEST_ASSERT_FALSE(ring_buffer_init(NULL, mem, 16));
    TEST_ASSERT_FALSE(ring_buffer_init(&test_rb, NULL, 16));
    TEST_ASSERT_FALSE(ring_buffer_init(&test_rb, mem, 0));
}

void test_ring_buffer_init_non_power_of_two_fails(void)
{
    Ring_Buffer_t test_rb;
    uint8_t mem[30];

    TEST_ASSERT_FALSE(ring_buffer_init(&test_rb, mem, 15));
    TEST_ASSERT_FALSE(ring_buffer_init(&test_rb, mem, 17));
    TEST_ASSERT_FALSE(ring_buffer_init(&test_rb, mem, 30));
    TEST_ASSERT_TRUE(ring_buffer_init(&test_rb, mem, 16));
    TEST_ASSERT_TRUE(ring_buffer_init(&test_rb, mem, 32));
}

void test_ring_buffer_put_and_get_single(void)
{
    uint8_t byte_out = 0;

    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_EQUAL_UINT32(0, ring_buffer_count(&rb));
    TEST_ASSERT_EQUAL_UINT32(TEST_BUF_SIZE, ring_buffer_free_space(&rb));

    TEST_ASSERT_TRUE(ring_buffer_put(&rb, 0x42));
    TEST_ASSERT_FALSE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_EQUAL_UINT32(1, ring_buffer_count(&rb));
    TEST_ASSERT_EQUAL_UINT32(TEST_BUF_SIZE - 1, ring_buffer_free_space(&rb));

    TEST_ASSERT_TRUE(ring_buffer_get(&rb, &byte_out));
    TEST_ASSERT_EQUAL_HEX8(0x42, byte_out);
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_get_empty_fails(void)
{
    uint8_t byte_out = 0;
    TEST_ASSERT_FALSE(ring_buffer_get(&rb, &byte_out));
}

void test_ring_buffer_fill_to_capacity(void)
{
    uint8_t byte_out = 0;

    for (uint8_t i = 0; i < TEST_BUF_SIZE; i++) {
        TEST_ASSERT_TRUE(ring_buffer_put(&rb, i));
    }

    TEST_ASSERT_TRUE(ring_buffer_is_full(&rb));
    TEST_ASSERT_EQUAL_UINT32(TEST_BUF_SIZE, ring_buffer_count(&rb));
    TEST_ASSERT_EQUAL_UINT32(0, ring_buffer_free_space(&rb));

    // Next put must fail because buffer is full
    TEST_ASSERT_FALSE(ring_buffer_put(&rb, 0xFF));

    // Drain all and verify FIFO order
    for (uint8_t i = 0; i < TEST_BUF_SIZE; i++) {
        TEST_ASSERT_TRUE(ring_buffer_get(&rb, &byte_out));
        TEST_ASSERT_EQUAL_HEX8(i, byte_out);
    }
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_peek(void)
{
    uint8_t peek_val = 0;
    uint8_t get_val = 0;

    TEST_ASSERT_FALSE(ring_buffer_peek(&rb, &peek_val));

    ring_buffer_put(&rb, 0xAA);
    TEST_ASSERT_TRUE(ring_buffer_peek(&rb, &peek_val));
    TEST_ASSERT_EQUAL_HEX8(0xAA, peek_val);

    // Count should still be 1 after peek
    TEST_ASSERT_EQUAL_UINT32(1, ring_buffer_count(&rb));

    TEST_ASSERT_TRUE(ring_buffer_get(&rb, &get_val));
    TEST_ASSERT_EQUAL_HEX8(0xAA, get_val);
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_write_and_read_multiple(void)
{
    const uint8_t data_in[6] = {10, 20, 30, 40, 50, 60};
    uint8_t data_out[6] = {0};

    size_t written = ring_buffer_write(&rb, data_in, 6);
    TEST_ASSERT_EQUAL_UINT32(6, written);
    TEST_ASSERT_EQUAL_UINT32(6, ring_buffer_count(&rb));

    size_t read_bytes = ring_buffer_read(&rb, data_out, 6);
    TEST_ASSERT_EQUAL_UINT32(6, read_bytes);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(data_in, data_out, 6);
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_wrap_around_continuity(void)
{
    // Advance pointers past initial indices by putting and getting
    for (int cycle = 0; cycle < 50; cycle++) {
        TEST_ASSERT_TRUE(ring_buffer_put(&rb, (uint8_t)cycle));
        uint8_t val = 0;
        TEST_ASSERT_TRUE(ring_buffer_get(&rb, &val));
        TEST_ASSERT_EQUAL_HEX8((uint8_t)cycle, val);
    }
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_reset(void)
{
    ring_buffer_put(&rb, 1);
    ring_buffer_put(&rb, 2);
    TEST_ASSERT_EQUAL_UINT32(2, ring_buffer_count(&rb));

    ring_buffer_reset(&rb);
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_EQUAL_UINT32(0, ring_buffer_count(&rb));
}
