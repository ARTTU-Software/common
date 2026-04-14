#include "unity.h"
#include "generic_fsm_driver.h"
#include <string.h>

TEST_SOURCE_FILE("generic_fsm_driver.c");

typedef enum {
	TEST_STATE_IDLE = 0,
	TEST_STATE_RUN = 1,
	TEST_STATE_FAULT = 2,
	TEST_STATE_COUNT
} TestState_t;

typedef enum {
	TEST_REASON_NONE = 0,
	TEST_REASON_START = 1,
	TEST_REASON_ERROR = 2
} TestReason_t;

typedef struct {
	uint8_t start;
	uint8_t error;
} TestEvents_t;

static FSM_Driver_t test_driver;
static FSM_State_Config_t test_state_configs[TEST_STATE_COUNT];
static TestEvents_t test_events;

static uint8_t build_snapshot_call_count;
static uint8_t decide_call_count;
static uint8_t entry_call_count;
static uint8_t exit_call_count;
static uint8_t action_call_count;

static FSM_State_t last_entry_state;
static FSM_State_t last_exit_state;
static FSM_State_t last_action_state;

static void test_build_snapshot(FSM_Event_Snapshot_t* events)
{
	TestEvents_t* event_ptr = (TestEvents_t*)(*events);
	build_snapshot_call_count++;

	if (event_ptr != NULL) {
		event_ptr->start = 1U;
	}
}

static FSM_State_t test_decide_next_state(
	FSM_State_t current_state,
	const FSM_Event_Snapshot_t* events,
	FSM_Reason_t* reason)
{
	const TestEvents_t* event_ptr = (const TestEvents_t*)(*events);
	decide_call_count++;

	if (event_ptr != NULL && event_ptr->error != 0U) {
		*reason = TEST_REASON_ERROR;
		return TEST_STATE_FAULT;
	}

	if (event_ptr != NULL && event_ptr->start != 0U && current_state == TEST_STATE_IDLE) {
		*reason = TEST_REASON_START;
		return TEST_STATE_RUN;
	}

	*reason = TEST_REASON_NONE;
	return current_state;
}

static void test_entry_action(FSM_State_t state)
{
	entry_call_count++;
	last_entry_state = state;
}

static void test_exit_action(FSM_State_t state)
{
	exit_call_count++;
	last_exit_state = state;
}

static void test_state_action(FSM_State_t state)
{
	action_call_count++;
	last_action_state = state;
}

void setUp(void)
{
	memset(&test_driver, 0, sizeof(FSM_Driver_t));
	memset(&test_state_configs, 0, sizeof(test_state_configs));
	memset(&test_events, 0, sizeof(TestEvents_t));

	build_snapshot_call_count = 0U;
	decide_call_count = 0U;
	entry_call_count = 0U;
	exit_call_count = 0U;
	action_call_count = 0U;

	last_entry_state = 0U;
	last_exit_state = 0U;
	last_action_state = 0U;

	test_state_configs[TEST_STATE_IDLE].entry_action = test_entry_action;
	test_state_configs[TEST_STATE_IDLE].exit_action = test_exit_action;
	test_state_configs[TEST_STATE_IDLE].action = test_state_action;

	test_state_configs[TEST_STATE_RUN].entry_action = test_entry_action;
	test_state_configs[TEST_STATE_RUN].exit_action = test_exit_action;
	test_state_configs[TEST_STATE_RUN].action = test_state_action;

	test_state_configs[TEST_STATE_FAULT].entry_action = test_entry_action;
	test_state_configs[TEST_STATE_FAULT].exit_action = test_exit_action;
	test_state_configs[TEST_STATE_FAULT].action = test_state_action;
}

void tearDown(void)
{
}

void test_FSM_init_should_initialize_tracking_and_configuration(void)
{
	FSM_init(
		&test_driver,
		test_decide_next_state,
		test_build_snapshot,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_IDLE, test_driver.tracking.current_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_IDLE, test_driver.tracking.next_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_REASON_NONE, test_driver.tracking.last_reason);
	TEST_ASSERT_EQUAL_UINT32(0U, test_driver.tracking.transition_count);
	TEST_ASSERT_EQUAL_UINT8(0U, test_driver.tracking.fault_latched);
	TEST_ASSERT_EQUAL_PTR(test_decide_next_state, test_driver.decide_next_state_fn);
	TEST_ASSERT_EQUAL_PTR(test_build_snapshot, test_driver.build_event_snapshot_fn);
	TEST_ASSERT_EQUAL_PTR(test_state_configs, test_driver.state_configs);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_COUNT, test_driver.num_states);
	TEST_ASSERT_EQUAL_PTR(&test_events, test_driver.event_snapshot);
}

void test_FSM_init_with_null_driver_should_not_crash(void)
{
	FSM_init(
		NULL,
		test_decide_next_state,
		test_build_snapshot,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	TEST_PASS();
}

void test_FSM_step_should_transition_and_run_exit_entry_and_action(void)
{
	uint8_t state_changed;

	FSM_init(
		&test_driver,
		test_decide_next_state,
		test_build_snapshot,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	test_events.start = 1U;
	state_changed = FSM_step(&test_driver);

	TEST_ASSERT_EQUAL_UINT8(1U, state_changed);
	TEST_ASSERT_EQUAL_UINT8(1U, build_snapshot_call_count);
	TEST_ASSERT_EQUAL_UINT8(1U, decide_call_count);
	TEST_ASSERT_EQUAL_UINT8(1U, exit_call_count);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_IDLE, last_exit_state);
	TEST_ASSERT_EQUAL_UINT8(1U, entry_call_count);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, last_entry_state);
	TEST_ASSERT_EQUAL_UINT8(1U, action_call_count);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, last_action_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, test_driver.tracking.current_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, test_driver.tracking.next_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_REASON_START, test_driver.tracking.last_reason);
	TEST_ASSERT_EQUAL_UINT32(1U, test_driver.tracking.transition_count);
}

void test_FSM_step_without_transition_should_run_only_state_action(void)
{
	uint8_t state_changed;

	FSM_init(
		&test_driver,
		test_decide_next_state,
		NULL,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	state_changed = FSM_step(&test_driver);

	TEST_ASSERT_EQUAL_UINT8(0U, state_changed);
	TEST_ASSERT_EQUAL_UINT8(0U, build_snapshot_call_count);
	TEST_ASSERT_EQUAL_UINT8(1U, decide_call_count);
	TEST_ASSERT_EQUAL_UINT8(0U, exit_call_count);
	TEST_ASSERT_EQUAL_UINT8(0U, entry_call_count);
	TEST_ASSERT_EQUAL_UINT8(1U, action_call_count);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_IDLE, last_action_state);
	TEST_ASSERT_EQUAL_UINT8(TEST_REASON_NONE, test_driver.tracking.last_reason);
	TEST_ASSERT_EQUAL_UINT32(0U, test_driver.tracking.transition_count);
}

void test_FSM_step_with_null_driver_should_return_zero(void)
{
	uint8_t state_changed = FSM_step(NULL);
	TEST_ASSERT_EQUAL_UINT8(0U, state_changed);
}

void test_FSM_step_with_null_decision_function_should_return_zero(void)
{
	uint8_t state_changed;

	FSM_init(
		&test_driver,
		NULL,
		test_build_snapshot,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	state_changed = FSM_step(&test_driver);
	TEST_ASSERT_EQUAL_UINT8(0U, state_changed);
}

void test_FSM_step_with_null_state_configs_should_skip_actions_but_transition(void)
{
	uint8_t state_changed;

	FSM_init(
		&test_driver,
		test_decide_next_state,
		NULL,
		NULL,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	test_events.start = 1U;
	state_changed = FSM_step(&test_driver);

	TEST_ASSERT_EQUAL_UINT8(1U, state_changed);
	TEST_ASSERT_EQUAL_UINT8(0U, exit_call_count);
	TEST_ASSERT_EQUAL_UINT8(0U, entry_call_count);
	TEST_ASSERT_EQUAL_UINT8(0U, action_call_count);
	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, test_driver.tracking.current_state);
	TEST_ASSERT_EQUAL_UINT32(1U, test_driver.tracking.transition_count);
}

void test_FSM_getters_should_return_expected_values_and_zero_for_null(void)
{
	FSM_init(
		&test_driver,
		test_decide_next_state,
		NULL,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	test_events.start = 1U;
	(void)FSM_step(&test_driver);

	TEST_ASSERT_EQUAL_UINT8(TEST_STATE_RUN, FSM_get_current_state(&test_driver));
	TEST_ASSERT_EQUAL_UINT8(TEST_REASON_START, FSM_get_last_reason(&test_driver));
	TEST_ASSERT_EQUAL_UINT32(1U, FSM_get_transition_count(&test_driver));

	TEST_ASSERT_EQUAL_UINT8(0U, FSM_get_current_state(NULL));
	TEST_ASSERT_EQUAL_UINT8(0U, FSM_get_last_reason(NULL));
	TEST_ASSERT_EQUAL_UINT32(0U, FSM_get_transition_count(NULL));
}

void test_FSM_fault_latch_apis_should_set_check_and_reset_latch(void)
{
	FSM_init(
		&test_driver,
		test_decide_next_state,
		NULL,
		test_state_configs,
		TEST_STATE_COUNT,
		TEST_STATE_IDLE,
		&test_events);

	TEST_ASSERT_EQUAL_UINT8(0U, FSM_is_fault_latched(&test_driver));

	FSM_set_fault_latch(&test_driver);
	TEST_ASSERT_EQUAL_UINT8(1U, FSM_is_fault_latched(&test_driver));

	FSM_reset_fault_latch(&test_driver);
	TEST_ASSERT_EQUAL_UINT8(0U, FSM_is_fault_latched(&test_driver));

	FSM_set_fault_latch(NULL);
	FSM_reset_fault_latch(NULL);
	TEST_ASSERT_EQUAL_UINT8(0U, FSM_is_fault_latched(NULL));
}
