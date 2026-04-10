#ifndef INC_GENERIC_FSM_DRIVER_H
#define INC_GENERIC_FSM_DRIVER_H

#include <stdint.h>

/**
 * @brief Opaque FSM state type. Application defines concrete state enum.
 */
typedef uint8_t FSM_State_t;

/**
 * @brief Opaque FSM transition reason type. Application defines concrete reason enum.
 */
typedef uint8_t FSM_Reason_t;

/**
 * @brief Opaque event snapshot type. Application structures contain actual events.
 */
typedef void* FSM_Event_Snapshot_t;

/**
 * @brief Function pointer for deciding next state based on events.
 * Takes current state and event snapshot, returns next state and sets reason.
 */
typedef FSM_State_t (*FSM_DecisionFn_t)(FSM_State_t current_state, const FSM_Event_Snapshot_t* events, FSM_Reason_t *reason);

/**
 * @brief Function pointer for building event snapshot from external inputs.
 * Application-specific; converts hardware/communication state into event struct.
 */
typedef void (*FSM_SnapshotBuildFn_t)(FSM_Event_Snapshot_t* events);

/**
 * @brief Function pointer for executing entry action on state transition.
 */
typedef void (*FSM_EntryActionFn_t)(FSM_State_t state);

/**
 * @brief Function pointer for executing exit action on state transition.
 */
typedef void (*FSM_ExitActionFn_t)(FSM_State_t state);

/**
 * @brief Function pointer for executing state behavior action each cycle.
 */
typedef void (*FSM_ActionFn_t)(FSM_State_t state);

/**
 * @brief Configuration for a single FSM state's callbacks.
 */
typedef struct {
    FSM_EntryActionFn_t entry_action;
    FSM_ExitActionFn_t exit_action;
    FSM_ActionFn_t action;
} FSM_State_Config_t;

/**
 * @brief Observability and state tracking structure.
 */
typedef struct {
    FSM_State_t current_state;
    FSM_State_t next_state;
    FSM_Reason_t last_reason;
    uint32_t transition_count;
    uint8_t fault_latched;  // Once set, remains set until hard reset (for safety-critical faults)
} FSM_State_Tracking_t;

/**
 * @brief Main FSM driver structure holding behavior and state.
 */
typedef struct {
    // Current and next state tracking
    FSM_State_Tracking_t tracking;

    // Decision and event building logic
    FSM_DecisionFn_t decide_next_state_fn;
    FSM_SnapshotBuildFn_t build_event_snapshot_fn;

    // State action callbacks (array indexed by state value)
    FSM_State_Config_t* state_configs;
    uint8_t num_states;

    // Event snapshot buffer (application allocates and passes)
    FSM_Event_Snapshot_t event_snapshot;

} FSM_Driver_t;

/**
 * @brief Initialize FSM driver with decision function and state configs.
 * @param driver Driver instance.
 * @param decide_fn Transition decision function.
 * @param build_snapshot_fn Event snapshot builder function.
 * @param state_configs Array of state action configs.
 * @param num_states Number of states in the FSM.
 * @param initial_state Starting state.
 * @param event_snapshot Pointer to event snapshot struct (app-allocated).
 */
void FSM_init(FSM_Driver_t* driver,
    FSM_DecisionFn_t decide_fn,
    FSM_SnapshotBuildFn_t build_snapshot_fn,
    FSM_State_Config_t* state_configs,
    uint8_t num_states,
    FSM_State_t initial_state,
    FSM_Event_Snapshot_t event_snapshot);

/**
 * @brief Execute one FSM cycle: build events, decide next state, run actions.
 * @param driver Driver instance.
 * @return 1 if state changed, 0 if state unchanged.
 */
uint8_t FSM_step(FSM_Driver_t* driver);

/**
 * @brief Get current state.
 */
FSM_State_t FSM_get_current_state(const FSM_Driver_t* driver);

/**
 * @brief Get last transition reason.
 */
FSM_Reason_t FSM_get_last_reason(const FSM_Driver_t* driver);

/**
 * @brief Get transition count.
 */
uint32_t FSM_get_transition_count(const FSM_Driver_t* driver);

/**
 * @brief Set fault latch (permanent until reset).
 */
void FSM_set_fault_latch(FSM_Driver_t* driver);

/**
 * @brief Check if fault is latched.
 */
uint8_t FSM_is_fault_latched(const FSM_Driver_t* driver);

/**
 * @brief Reset fault latch (typically only on power-on or explicit hard reset).
 */
void FSM_reset_fault_latch(FSM_Driver_t* driver);

#endif /* INC_GENERIC_FSM_DRIVER_H */
