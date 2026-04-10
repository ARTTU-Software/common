#include "generic_fsm_driver.h"
#include <stddef.h>

void FSM_init(FSM_Driver_t* driver,
    FSM_DecisionFn_t decide_fn,
    FSM_SnapshotBuildFn_t build_snapshot_fn,
    FSM_State_Config_t* state_configs,
    uint8_t num_states,
    FSM_State_t initial_state,
    FSM_Event_Snapshot_t event_snapshot)
{
    if (driver == NULL) {
        return;
    }

    driver->tracking.current_state = initial_state;
    driver->tracking.next_state = initial_state;
    driver->tracking.last_reason = 0U;
    driver->tracking.transition_count = 0U;
    driver->tracking.fault_latched = 0U;

    driver->decide_next_state_fn = decide_fn;
    driver->build_event_snapshot_fn = build_snapshot_fn;
    driver->state_configs = state_configs;
    driver->num_states = num_states;
    driver->event_snapshot = event_snapshot;
}

uint8_t FSM_step(FSM_Driver_t* driver)
{
    if (driver == NULL || driver->decide_next_state_fn == NULL) {
        return 0;
    }

    // Build event snapshot
    if (driver->build_event_snapshot_fn != NULL) {
        driver->build_event_snapshot_fn(&driver->event_snapshot);
    }

    // Decide next state
    driver->tracking.next_state = driver->decide_next_state_fn(
        driver->tracking.current_state,
        &driver->event_snapshot,
        &driver->tracking.last_reason);

    // Check for state change
    uint8_t state_changed = (driver->tracking.next_state != driver->tracking.current_state);

    if (state_changed) {
        // Run exit action of current state
        if (driver->state_configs != NULL &&
            driver->tracking.current_state < driver->num_states &&
            driver->state_configs[driver->tracking.current_state].exit_action != NULL) {
            driver->state_configs[driver->tracking.current_state].exit_action(driver->tracking.current_state);
        }

        // Commit state and increment counter
        driver->tracking.current_state = driver->tracking.next_state;
        driver->tracking.transition_count++;

        // Run entry action of new state
        if (driver->state_configs != NULL &&
            driver->tracking.current_state < driver->num_states &&
            driver->state_configs[driver->tracking.current_state].entry_action != NULL) {
            driver->state_configs[driver->tracking.current_state].entry_action(driver->tracking.current_state);
        }
    }

    // Run action behavior of current state
    if (driver->state_configs != NULL &&
        driver->tracking.current_state < driver->num_states &&
        driver->state_configs[driver->tracking.current_state].action != NULL) {
        driver->state_configs[driver->tracking.current_state].action(driver->tracking.current_state);
    }

    return state_changed;
}

FSM_State_t FSM_get_current_state(const FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return 0;
    }
    return driver->tracking.current_state;
}

FSM_Reason_t FSM_get_last_reason(const FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return 0;
    }
    return driver->tracking.last_reason;
}

uint32_t FSM_get_transition_count(const FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return 0;
    }
    return driver->tracking.transition_count;
}

void FSM_set_fault_latch(FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return;
    }
    driver->tracking.fault_latched = 1U;
}

uint8_t FSM_is_fault_latched(const FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return 0;
    }
    return driver->tracking.fault_latched;
}

void FSM_reset_fault_latch(FSM_Driver_t* driver)
{
    if (driver == NULL) {
        return;
    }
    driver->tracking.fault_latched = 0U;
}
