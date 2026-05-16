---
title: Common - FSM
description: Generic finite state machine driver.
---

# FSM Driver

The FSM driver provides a small framework for state transitions with entry, exit, and action callbacks. The application owns state enums and event snapshots.

### Functions
#### `void FSM_init(FSM_Driver_t* driver, FSM_DecisionFn_t decide_fn, FSM_SnapshotBuildFn_t build_snapshot_fn, FSM_State_Config_t* state_configs, uint8_t num_states, FSM_State_t initial_state, FSM_Event_Snapshot_t event_snapshot)`
Initializes the FSM driver with decision logic and state callbacks.

#### `uint8_t FSM_step(FSM_Driver_t* driver)`
Runs one FSM cycle. Returns 1 when a state change occurs.

#### `FSM_State_t FSM_get_current_state(const FSM_Driver_t* driver)`
Returns the current state.

#### `FSM_Reason_t FSM_get_last_reason(const FSM_Driver_t* driver)`
Returns the last transition reason.

#### `uint32_t FSM_get_transition_count(const FSM_Driver_t* driver)`
Returns the number of transitions.

#### `void FSM_set_fault_latch(FSM_Driver_t* driver)`
Sets the fault latch until reset.

#### `uint8_t FSM_is_fault_latched(const FSM_Driver_t* driver)`
Returns 1 if a fault is latched.

#### `void FSM_reset_fault_latch(FSM_Driver_t* driver)`
Clears the fault latch.

#### `void FSM_request_bootloader(FSM_Driver_t* driver)`
Flags a bootloader request for the next cycle.

#### `void FSM_request_mode_change(FSM_Driver_t* driver, FSM_State_t requested_mode, FSM_Reason_t reason)`
Requests a mode change with a reason code.

### Usage
```c
#include "generic_fsm_driver.h"

typedef enum { STATE_IDLE = 0, STATE_RUN } AppState_t;
typedef enum { REASON_NONE = 0, REASON_START } AppReason_t;

typedef struct {
    uint8_t start_button;
} AppEvents_t;

static AppEvents_t events;

static FSM_State_t decide_state(FSM_State_t current, const FSM_Event_Snapshot_t* snapshot, FSM_Reason_t* reason) {
    const AppEvents_t* e = (const AppEvents_t*)snapshot;
    if (current == STATE_IDLE && e->start_button) {
        *reason = REASON_START;
        return STATE_RUN;
    }
    return current;
}

static void build_snapshot(FSM_Event_Snapshot_t* snapshot) {
    (void)snapshot;
    events.start_button = 1;
}

static FSM_State_Config_t state_cfg[] = {
    [STATE_IDLE] = { .entry_action = 0, .exit_action = 0, .action = 0 },
    [STATE_RUN] = { .entry_action = 0, .exit_action = 0, .action = 0 },
};

static FSM_Driver_t fsm;

void fsm_init(void) {
    FSM_init(&fsm, decide_state, build_snapshot, state_cfg, 2, STATE_IDLE, &events);
}

void fsm_step(void) {
    (void)FSM_step(&fsm);
}
```
