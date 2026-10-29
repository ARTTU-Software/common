#include "unity.h"
#include "can_driver_cases.h"

TEST_SOURCE_FILE("can_driver_core.c");
TEST_SOURCE_FILE("can_driver_cases.c");

void setUp(void) {}
void tearDown(void) {}

void test_send_now_submits_each_event_immediately(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(1U));
}

void test_send_now_returns_busy_without_replaying_old_status(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(2U));
}

void test_periodic_coalesces_stalls_and_preserves_phase(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(3U));
}

void test_periodic_retries_latest_demand_after_backpressure(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(4U));
}

void test_periodic_budget_rotates_between_due_slots(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(5U));
}

void test_periodic_handles_wrapping_millisecond_clock(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(6U));
}

void test_multiple_buses_have_independent_submission_state(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(7U));
}

void test_rx_preserves_copies_timestamps_and_overflow_order(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(8U));
}

void test_rx_rejects_invalid_length_id_and_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(9U));
}

void test_init_rejects_incomplete_configuration(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(10U));
}

void test_standard_and_extended_ids_remain_distinct(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(11U));
}

void test_status_event_can_be_sent_before_due_periodic_demand(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(12U));
}

void test_helpers_preserve_byte_order_constant_configuration_and_rate_bounds(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, can_driver_run_case(13U));
}
