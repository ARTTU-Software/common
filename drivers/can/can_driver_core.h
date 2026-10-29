#ifndef INC_CAN_DRIVER_CORE_H
#define INC_CAN_DRIVER_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include "can_helpers.h"

#define CAN_DRIVER_PAYLOAD_BYTES 8U

/** @brief Results distinguish backpressure from invalid input and port failure. */
typedef enum {
    CAN_DRIVER_OK = 0,
    CAN_DRIVER_BUSY,
    CAN_DRIVER_EMPTY,
    CAN_DRIVER_INVALID,
    CAN_DRIVER_NOT_INITIALIZED,
    CAN_DRIVER_IO_ERROR
} can_driver_status_t;

/** @brief Classic CAN data frame. Length is bytes, never a vendor DLC encoding. */
typedef struct {
    uint32_t id;
    bool extended;
    uint8_t length;
    uint8_t data[CAN_DRIVER_PAYLOAD_BYTES];
} can_driver_frame_t;

/** @brief Copied input with the producer's original reception time in milliseconds. */
typedef struct {
    can_driver_frame_t frame;
    uint32_t received_ms;
} can_driver_rx_frame_t;

/**
 * @brief One latest-value periodic message, stored in caller-owned static memory.
 * Initialize frame and period_ms before init. Afterwards use update_periodic()
 * for payload changes; the driver owns previous_ms and the scheduling phase.
 */
typedef struct {
    can_driver_frame_t frame;
    uint32_t period_ms;
    uint32_t previous_ms;
} can_driver_periodic_t;

/**
 * @brief Nonblocking hardware handoff. Copy the frame before returning OK.
 * Return BUSY without submitting anything when hardware has no space, or
 * IO_ERROR on failure. OK means accepted by hardware, not acknowledged on bus.
 * The callback may apply a final application authorization check before sending.
 */
typedef can_driver_status_t (*can_driver_submit_fn_t)(
    void *context, const can_driver_frame_t *frame);

/** @brief Configuration and storage must remain alive until the driver is idle. */
typedef struct {
    can_driver_submit_fn_t submit;
    void *context;
    can_driver_periodic_t *periodic;
    uint16_t periodic_count;
    uint16_t tx_budget; /**< Maximum submissions per periodic call; zero uses count. */
    can_driver_rx_frame_t *rx_storage;
    uint16_t rx_capacity; /**< Allocated slots, with one reserved. Zero disables RX. */
} can_driver_config_t;

/** @brief Per-instance counts. TX counts hardware acceptance, not bus delivery. */
typedef struct {
    uint32_t tx_submitted;
    uint32_t tx_busy;
    uint32_t tx_errors;
    uint32_t rx_received;
    uint32_t rx_dropped;
    uint32_t rx_invalid;
} can_driver_stats_t;

/**
 * @brief Driver state. Zero-initialize and access it only through this API.
 * Init, TX, updates, pop and statistics have one serialized task/main owner.
 * RX ingress has exactly one ISR producer. The RX ring uses release/acquire
 * publication; task mutexes and HAL types are not dependencies of the core.
 */
typedef struct {
    can_driver_config_t config;
    uint16_t next_periodic;
    bool initialized;
    bool tx_active;
    can_driver_stats_t tx_stats;
    atomic_uint rx_head;
    atomic_uint rx_tail;
    atomic_uint rx_received;
    atomic_uint rx_dropped;
    atomic_uint rx_invalid;
} can_driver_t;

/**
 * @brief Validate storage and start periodic phases at now_ms (milliseconds).
 * First periodic transmission is due after one period. Periods must be in
 * 1..INT32_MAX ms. Call only while the task and ISR are idle. Failed init leaves
 * the instance closed. Configuration and driver must not overlap in memory.
 * @return OK or INVALID for missing storage, callback, invalid frame or period.
 */
can_driver_status_t can_driver_init(can_driver_t *driver,
    const can_driver_config_t *config, uint32_t now_ms);

/**
 * @brief Attempt due periodic frames once each, within the configured TX budget.
 * Call from the owning task/main loop with wrapping uint32_t milliseconds.
 * Stalls coalesce to one latest frame; successful sends preserve the original
 * phase. BUSY/error leaves that slot due for retry. Service at least once per
 * timer wrap. Rotation prevents a full hardware FIFO starving later slots.
 * @return OK (also when none are due), BUSY, IO_ERROR or NOT_INITIALIZED.
 */
can_driver_status_t can_driver_send_periodic(can_driver_t *driver, uint32_t now_ms);

/**
 * @brief Attempt this frame immediately, without queueing or waiting.
 * The caller retains/retries an event on BUSY or error. No stale software copy
 * is kept. This cannot overtake or cancel frames already accepted by hardware.
 * @return OK, BUSY, IO_ERROR, INVALID or NOT_INITIALIZED.
 */
can_driver_status_t can_driver_send_now(can_driver_t *driver,
    const can_driver_frame_t *frame);

/**
 * @brief Copy the latest payload into a periodic slot without changing its phase.
 * @param index Zero-based slot. ID and standard/extended format must match it.
 * @return OK, INVALID or NOT_INITIALIZED. Call from the owning task only.
 */
can_driver_status_t can_driver_update_periodic(can_driver_t *driver,
    uint16_t index, const can_driver_frame_t *frame);

/**
 * @brief Copy a frame from the sole RX ISR producer; never blocks.
 * @param received_ms Original reception time in milliseconds.
 * @return OK, BUSY when full (drops newest), INVALID or NOT_INITIALIZED.
 */
can_driver_status_t can_driver_receive_isr(can_driver_t *driver,
    const can_driver_frame_t *frame, uint32_t received_ms);

/**
 * @brief Copy the oldest received frame to caller-owned output in task context.
 * @return OK, EMPTY, INVALID or NOT_INITIALIZED. Output is unchanged on failure.
 */
can_driver_status_t can_driver_receive(can_driver_t *driver,
    can_driver_rx_frame_t *out_frame);

/**
 * @brief Copy per-instance diagnostics from the owning task.
 * RX counters are individually atomic; the snapshot can span ISR arrivals.
 * @return OK, INVALID or NOT_INITIALIZED.
 */
can_driver_status_t can_driver_get_stats(const can_driver_t *driver,
    can_driver_stats_t *out_stats);

#endif /* INC_CAN_DRIVER_CORE_H */
