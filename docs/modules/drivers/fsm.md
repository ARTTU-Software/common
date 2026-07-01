---
title: Common - FSM
description: Generic finite state machine driver.
---

# FSM Driver

The FSM driver provides a small framework for state transitions with entry, exit, and action callbacks. The application owns the state enum, reason enum, and event snapshot.

> [!NOTE]
> This pattern is used in CAN-Gateway through `board_fsm.c` and `task_board_fsm_actions.c`.

## Structure
> [!NOTE]
> You should have a `task_board_fsm.c` and `task_board_fsm_actions.c` pair of files to manage the driver setup and the state-specific actions.

## State and Flow Diagram

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
    %% Subgraph Styles
    style Transition_Path fill:#0f172a,stroke:#10b981,stroke-width:2px,color:#fff
    style No_Transition_Path fill:#0f172a,stroke:#64748b,stroke-width:2px,color:#fff
    style Optional_Branches fill:#0f172a,stroke:#a1a1aa,stroke-width:2px,color:#fff

    %% Node Styles
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

    %% Feedback loop back to FSM_step()
    handle_fault -->|Repeat Next Cycle| begin_cycle
    trigger_boot -->|Repeat Next Cycle| begin_cycle
    boot_req -- No --> begin_cycle
```

</div>

## Functions
#### `void FSM_init(FSM_Driver_t* driver, FSM_DecisionFn_t decide_fn, FSM_SnapshotBuildFn_t build_snapshot_fn, FSM_State_Config_t* state_configs, uint8_t num_states, FSM_State_t initial_state, FSM_Event_Snapshot_t event_snapshot)`
Initializes the FSM driver with decision logic, state callbacks, and the event snapshot storage.

#### `uint8_t FSM_step(FSM_Driver_t* driver)`
Runs one FSM cycle. It builds events, decides the next state, runs exit and entry hooks when needed, and then runs the current state's action.

#### `FSM_State_t FSM_get_current_state(const FSM_Driver_t* driver)`
Returns the current FSM state.

#### `FSM_Reason_t FSM_get_last_reason(const FSM_Driver_t* driver)`
Returns the last transition reason.

#### `uint32_t FSM_get_transition_count(const FSM_Driver_t* driver)`
Returns the number of transitions that have occurred.

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

## CAN-Gateway example
For an example `task_board_fsm.c` file:
```c
FSM_Driver_t board_fsm_driver;

static Board_FSM_Event_Snapshot_t board_fsm_event_snapshot;

static FSM_State_Config_t board_fsm_state_configs[BOARD_FSM_MODE_COUNT] = {
    [BOARD_FSM_MODE_INIT] = {NULL, NULL, NULL},
    [BOARD_FSM_MODE_IDLE] = {board_fsm_mode_idle_entry, board_fsm_mode_idle_exit, board_fsm_mode_idle_action},
    [BOARD_FSM_MODE_PROCESS_SENSORS] = {board_fsm_mode_process_sensors_entry, board_fsm_mode_process_sensors_exit, board_fsm_mode_process_sensors_action},
    [BOARD_FSM_MODE_OUTPUT_SENSORS] = {board_fsm_mode_output_sensors_entry, board_fsm_mode_output_sensors_exit, board_fsm_mode_output_sensors_action},
    [BOARD_FSM_MODE_BOOTLOADER] = {board_fsm_mode_bootloader_entry, board_fsm_mode_bootloader_exit, board_fsm_mode_bootloader_action},
    [BOARD_FSM_MODE_FAULT] = {board_fsm_mode_fault_entry, board_fsm_mode_fault_exit, board_fsm_mode_fault_action},
};

void board_fsm_init(void)
{
    FSM_init(&board_fsm_driver,
        board_fsm_decide_mode,
        board_fsm_build_events,
        board_fsm_state_configs,
        BOARD_FSM_MODE_COUNT,
        BOARD_FSM_MODE_INIT,
        (FSM_Event_Snapshot_t)&board_fsm_event_snapshot);
}
```

### CAN-Gateway decision logic
The board FSM is driven from the generic layer, but the CAN-Gateway decides transitions with board-specific events.

```c
FSM_State_t board_fsm_decide_mode(FSM_State_t current_mode, const FSM_Event_Snapshot_t *events, FSM_Reason_t *reason)
{
    if (FSM_is_fault_latched(&board_fsm_driver))
    {
        return BOARD_FSM_MODE_FAULT;
    }

    if (events == NULL || *events == NULL)
    {
        if (reason != NULL)
        {
            *reason = BOARD_FSM_REASON_INVALID_EVENT;
        }
        return BOARD_FSM_MODE_FAULT;
    }

    const Board_FSM_Event_Snapshot_t *board_events = (const Board_FSM_Event_Snapshot_t *)(*events);

    if (board_events->bootloader_requested != 0U)
    {
        if (reason != NULL)
        {
            *reason = BOARD_FSM_REASON_BOOTLOADER_REQUEST;
        }
        return BOARD_FSM_MODE_BOOTLOADER;
    }

    switch ((Board_FSM_Mode_t)current_mode)
    {
        case BOARD_FSM_MODE_INIT:
            if (reason != NULL)
            {
                *reason = BOARD_FSM_REASON_INIT_COMPLETE;
            }
            return BOARD_FSM_MODE_IDLE;

        case BOARD_FSM_MODE_IDLE:
            if (board_events->adc_ready != 0U)
            {
                if (reason != NULL)
                {
                    *reason = BOARD_FSM_REASON_ADC_READY;
                }
                return BOARD_FSM_MODE_PROCESS_SENSORS;
            }
            return BOARD_FSM_MODE_IDLE;

        case BOARD_FSM_MODE_PROCESS_SENSORS:
            if (reason != NULL)
            {
                *reason = BOARD_FSM_REASON_PROCESS_SENSORS_DONE;
            }
            return BOARD_FSM_MODE_OUTPUT_SENSORS;

        case BOARD_FSM_MODE_OUTPUT_SENSORS:
            if (reason != NULL)
            {
                *reason = BOARD_FSM_REASON_OUTPUT_SENSORS_COMPLETE;
            }
            return BOARD_FSM_MODE_IDLE;

        case BOARD_FSM_MODE_BOOTLOADER:
            return BOARD_FSM_MODE_BOOTLOADER;

        case BOARD_FSM_MODE_FAULT:
        default:
            return BOARD_FSM_MODE_FAULT;
    }
}
```

### Main loop usage
For the CAN-Gateway main loop, the FSM is stepped after the CAN processing task:

```c
board_fsm_init();

while (1)
{
    process_can_helper();
    FSM_step(&board_fsm_driver);
}
```
