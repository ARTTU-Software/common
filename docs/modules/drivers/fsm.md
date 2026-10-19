---
title: Common - FSM
description: Generic finite state machine driver.
---

# FSM Driver

## Overview

The FSM driver provides generic state machine management. Implementation requires three components:

1. **Decision function**: evaluates current state and event snapshot data to determine state transitions.
2. **Event snapshot builder**: aggregates hardware, timer, and communication flags into a state snapshot before evaluation.
3. **State configuration table**: defines `entry`, `exit`, and `action` callback pointers per state.

During each cycle (`FSM_step`):
1. Snapshot builder updates event data.
2. Decision function evaluates next state.
3. If state change occurs: executes current state `exit` callback, updates active state tracking, and executes new state `entry` callback.
4. Executes active state `action` callback every step.

Cross-FSM requests are supported via `FSM_request_mode_change()`. An FSM can request state transitions on a target FSM, evaluated on the target FSM's next step.

---

## State and Flow Diagram

<details>
<summary>Click to expand FSM cycle diagram</summary>

<div data-zoom="0.9">

```mermaid
%%{init: {
  'theme': 'dark',
  'themeVariables': {
    'fontSize': '14px',
    'fontFamily': 'system-ui, sans-serif'
  },
  'flowchart': {
    'nodeSpacing': 30,
    'rankSpacing': 40,
    'padding': 15,
    'curve': 'basis'
  }
}}%%
flowchart TB
    style Transition_Path fill:#0f172a,stroke:#10b981,stroke-width:2px,color:#fff
    style No_Transition_Path fill:#0f172a,stroke:#64748b,stroke-width:2px,color:#fff
    style Optional_Branches fill:#0f172a,stroke:#a1a1aa,stroke-width:2px,color:#fff

    classDef genericLogic fill:#1d4ed8,stroke:#3b82f6,stroke-width:1px,color:#fff;
    classDef appLogic fill:#701a75,stroke:#d946ef,stroke-width:1px,color:#fff;
    classDef transPath fill:#065f46,stroke:#10b981,stroke-width:1px,color:#fff;
    classDef noTransPath fill:#334155,stroke:#64748b,stroke-width:1px,color:#fff;
    classDef optBranch fill:#78350f,stroke:#f59e0b,stroke-width:1px,color:#fff;
    classDef errorState fill:#991b1b,stroke:#ef4444,stroke-width:1px,color:#fff;
    classDef decisionPoint fill:#7c2d12,stroke:#f97316,stroke-width:1px,color:#fff;

    init_fsm(["FSM_init()"]):::genericLogic
    begin_cycle(["FSM_step()"]):::genericLogic
    snap_exists{"Snapshot Exist?"}:::decisionPoint
    build_snap["Build Event Snapshot"]:::appLogic
    decide_state["FSM_DecisionFn"]:::appLogic
    state_changed{"State Changed?"}:::decisionPoint

    init_fsm --> begin_cycle
    begin_cycle --> snap_exists
    snap_exists -- Yes --> build_snap
    snap_exists -- No --> decide_state
    build_snap --> decide_state
    decide_state --> state_changed

    subgraph Transition_Path ["Transition Path"]
        direction TB
        exit_act["Exit Action"]:::transPath
        commit_state["Commit State"]:::transPath
        entry_act["Entry Action"]:::transPath
        exit_act --> commit_state --> entry_act
    end

    subgraph No_Transition_Path ["No Transition Path"]
        direction TB
        skip_trans["Skip Transition"]:::noTransPath
    end

    state_changed -- Yes --> exit_act
    state_changed -- No --> skip_trans

    exec_action["Run Current State Action"]:::transPath
    entry_act --> exec_action
    skip_trans --> exec_action

    subgraph Optional_Branches ["Optional Branches"]
        direction TB
        fault_check{"Fault Latched?"}:::decisionPoint
        handle_fault["Handle Fault"]:::errorState
        boot_req{"Bootloader Requested?"}:::decisionPoint
        trigger_boot["Trigger Bootloader"]:::errorState

        fault_check -- Yes --> handle_fault
        fault_check -- No --> boot_req
        boot_req -- Yes --> trigger_boot
    end

    exec_action --> fault_check

    handle_fault -->|Repeat Next Cycle| begin_cycle
    trigger_boot -->|Repeat Next Cycle| begin_cycle
    boot_req -- No --> begin_cycle
```

</div>

</details>

---

## API Reference

#### `void FSM_init(driver, decide_fn, build_snapshot_fn, state_configs, num_states, initial_state, event_snapshot)`

Initializes the FSM instance. `event_snapshot` expects a pointer to application snapshot memory (cast to `FSM_Event_Snapshot_t`).

#### `uint8_t FSM_step(driver)`

Executes single FSM step: updates events, evaluates transitions, runs entry/exit callbacks when state changes occur, and calls active state action callback. Returns `1` on transition, `0` otherwise.

#### `FSM_State_t FSM_get_current_state(driver)`

Returns active FSM state.

#### `FSM_Reason_t FSM_get_last_reason(driver)`

Returns reason code associated with the most recent transition.

#### `uint32_t FSM_get_transition_count(driver)`

Returns total transition count since initialization.

#### `void FSM_set_fault_latch(driver)` / `uint8_t FSM_is_fault_latched(driver)` / `void FSM_reset_fault_latch(driver)`

Manages fault latch flag. Once latched, flag remains active until explicitly cleared via `FSM_reset_fault_latch()`.

#### `void FSM_request_bootloader(driver)`

Sets bootloader request flag on tracking structure.

#### `void FSM_request_mode_change(driver, requested_mode, reason)`

Queues an external transition request to `requested_mode` with reason code.

---

## Integration Pattern

FSM implementations utilize two source files per state machine:

| File | Contents |
|------|----------|
| `board_fsm.c` | State enum, snapshot struct, decision function, snapshot builder, init routine |
| `board_fsm_actions.c` | Entry, exit, and action callback implementations |

### FSM Configuration Example

```c
typedef enum {
    STATE_IDLE,
    STATE_ACTIVE,
    STATE_FAULT,
    STATE_COUNT
} My_State_t;

typedef struct {
    uint8_t sensor_ready;
    uint8_t fault_detected;
} My_Event_Snapshot_t;

static FSM_State_Config_t state_configs[STATE_COUNT] = {
    [STATE_IDLE]   = { idle_entry,   NULL, idle_action   },
    [STATE_ACTIVE] = { active_entry, NULL, active_action },
    [STATE_FAULT]  = { fault_entry,  NULL, fault_action  },
};

static FSM_State_t decide(FSM_State_t current, const FSM_Event_Snapshot_t *events, FSM_Reason_t *reason) {
    My_Event_Snapshot_t *snap = (My_Event_Snapshot_t *)(*events);

    if (snap->fault_detected) {
        *reason = REASON_FAULT;
        return STATE_FAULT;
    }
    if (current == STATE_IDLE && snap->sensor_ready) {
        *reason = REASON_SENSOR_READY;
        return STATE_ACTIVE;
    }
    return current;
}

static My_Event_Snapshot_t snapshot;
FSM_Driver_t my_fsm;

void my_fsm_init(void) {
    FSM_init(&my_fsm, decide, build_events, state_configs,
             STATE_COUNT, STATE_IDLE, (FSM_Event_Snapshot_t)&snapshot);
}

// Main loop step:
FSM_step(&my_fsm);
```

---

## Example: CAN-Gateway FSM

Single FSM implementation cycling through operational modes:

```
INIT -> IDLE -> PROCESS_SENSORS -> OUTPUT_SENSORS -> IDLE
```

<details>
<summary>CAN-Gateway decision function</summary>

```c
FSM_State_t board_fsm_decide_mode(FSM_State_t current_mode,
    const FSM_Event_Snapshot_t *events, FSM_Reason_t *reason)
{
    if (FSM_is_fault_latched(&board_fsm_driver))
        return BOARD_FSM_MODE_FAULT;

    const Board_FSM_Event_Snapshot_t *snap = (const Board_FSM_Event_Snapshot_t *)(*events);

    if (snap->bootloader_requested) {
        *reason = BOARD_FSM_REASON_BOOTLOADER_REQUEST;
        return BOARD_FSM_MODE_BOOTLOADER;
    }

    switch ((Board_FSM_Mode_t)current_mode) {
        case BOARD_FSM_MODE_INIT:
            *reason = BOARD_FSM_REASON_INIT_COMPLETE;
            return BOARD_FSM_MODE_IDLE;

        case BOARD_FSM_MODE_IDLE:
            if (snap->adc_ready) {
                *reason = BOARD_FSM_REASON_ADC_READY;
                return BOARD_FSM_MODE_PROCESS_SENSORS;
            }
            return BOARD_FSM_MODE_IDLE;

        case BOARD_FSM_MODE_PROCESS_SENSORS:
            *reason = BOARD_FSM_REASON_PROCESS_SENSORS_DONE;
            return BOARD_FSM_MODE_OUTPUT_SENSORS;

        case BOARD_FSM_MODE_OUTPUT_SENSORS:
            *reason = BOARD_FSM_REASON_OUTPUT_SENSORS_COMPLETE;
            return BOARD_FSM_MODE_IDLE;

        default:
            return BOARD_FSM_MODE_FAULT;
    }
}
```

</details>

---

## Example: ECU Multi-FSM Architecture

The ECU uses sub-FSM controllers managed by a master ECU state machine:

```
ECU FSM:  IDLE -> R2D -> CONTROL -> SHUTDOWN
                  |        |
                  v        v
            R2D FSM    Control FSM    Shutdown FSM
```

Master FSM entry callbacks request state changes on child FSM instances:

```c
// Master ECU FSM entry action for R2D state
void ecu_fsm_state_r2d_entry(FSM_State_t state) {
    (void)state;
    ecu_send_software_frame();
    FSM_request_mode_change(&r2d_fsm_driver, SYSTEM_R2D_FIRST_BTN, R2D_FSM_REASON_NONE);
}

// Master ECU FSM decision function
static FSM_State_t ecu_fsm_decide(FSM_State_t current_state,
    const FSM_Event_Snapshot_t *events, FSM_Reason_t *reason)
{
    ECU_FSM_Event_Snapshot_t *snap = (ECU_FSM_Event_Snapshot_t *)(*events);
    FSM_State_Tracking_t *tracking = &ecu_fsm_driver.tracking;

    if (snap->can_sdc_fault_requested) {
        *reason = ECU_FSM_REASON_FAULT_CAN_SDC;
        FSM_request_mode_change(&shutdown_fsm_driver,
            SYSTEM_SHUTDOWN_START, SHUTDOWN_FSM_REASON_SDC_FAULT);
        return SYSTEM_SHUTDOWN;
    }

    if (tracking->mode_change_requested) {
        *reason = tracking->mode_change_reason;
        tracking->mode_change_requested = 0U;
        return tracking->mode_change_state;
    }

    return current_state;
}
```

### Main Loop Execution

```c
r2d_fsm_init();
control_fsm_init();
shutdown_fsm_init();
ecu_fsm_init();

while (1) {
    process_can_frames(&can_driver);
    set_can_frames(&can_driver);
    service_can_tx();

    FSM_step(&ecu_fsm_driver);
}
```
