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

## State Diagram
![FSM Diagram Image](../../../../../public/images/FSM_driver_diagram.png)

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
