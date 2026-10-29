#include "can_driver_core.h"

#include <limits.h>
#include <stddef.h>

/* RX runs in an ISR, so the implementation must not fall back to a library lock. */
_Static_assert(ATOMIC_INT_LOCK_FREE == 2, "CAN RX needs lock-free unsigned atomics");

static bool can_driver_frame_valid(const can_driver_frame_t *frame)
{
    if (frame == NULL || frame->length > CAN_DRIVER_PAYLOAD_BYTES) {
        return false;
    }
    const uint32_t maximum_id = frame->extended ? 0x1fffffffU : 0x7ffU;
    return frame->id <= maximum_id;
}

can_driver_status_t can_driver_init(can_driver_t *driver,
    const can_driver_config_t *config, uint32_t now_ms)
{
    if (driver == NULL) {
        return CAN_DRIVER_INVALID;
    }
    driver->initialized = false;
    if (config == NULL || config->submit == NULL
        || (config->periodic_count != 0U && config->periodic == NULL)
        || (config->rx_capacity != 0U
            && (config->rx_capacity < 2U || config->rx_storage == NULL))) {
        return CAN_DRIVER_INVALID;
    }
    for (uint16_t i = 0U; i < config->periodic_count; i++) {
        const can_driver_periodic_t *slot = &config->periodic[i];
        if (!can_driver_frame_valid(&slot->frame)
            || slot->period_ms == 0U || slot->period_ms > INT32_MAX) {
            return CAN_DRIVER_INVALID;
        }
        for (uint16_t j = 0U; j < i; j++) {
            if (config->periodic[j].frame.id == slot->frame.id
                && config->periodic[j].frame.extended == slot->frame.extended) {
                return CAN_DRIVER_INVALID;
            }
        }
    }

    driver->config = *config;
    driver->next_periodic = 0U;
    driver->tx_active = false;
    driver->tx_stats = (can_driver_stats_t){0};
    atomic_init(&driver->rx_head, 0U);
    atomic_init(&driver->rx_tail, 0U);
    atomic_init(&driver->rx_received, 0U);
    atomic_init(&driver->rx_dropped, 0U);
    atomic_init(&driver->rx_invalid, 0U);
    for (uint16_t i = 0U; i < config->periodic_count; i++) {
        config->periodic[i].previous_ms = now_ms;
    }
    driver->initialized = true;
    return CAN_DRIVER_OK;
}

can_driver_status_t can_driver_send_now(can_driver_t *driver,
    const can_driver_frame_t *frame)
{
    if (driver == NULL || !can_driver_frame_valid(frame)) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    if (driver->tx_active) {
        driver->tx_stats.tx_busy++;
        return CAN_DRIVER_BUSY;
    }

    driver->tx_active = true;
    const can_driver_status_t status = driver->config.submit(driver->config.context, frame);
    driver->tx_active = false;
    if (status == CAN_DRIVER_OK) {
        driver->tx_stats.tx_submitted++;
    } else if (status == CAN_DRIVER_BUSY) {
        driver->tx_stats.tx_busy++;
    } else {
        driver->tx_stats.tx_errors++;
        return CAN_DRIVER_IO_ERROR;
    }
    return status;
}

can_driver_status_t can_driver_send_periodic(can_driver_t *driver, uint32_t now_ms)
{
    if (driver == NULL) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    const uint16_t count = driver->config.periodic_count;
    const uint16_t start = driver->next_periodic;
    uint16_t submitted = 0U;

    for (uint16_t i = 0U; i < count; i++) {
        const uint16_t index = (uint16_t)(((uint32_t)start + i) % count);
        can_driver_periodic_t *slot = &driver->config.periodic[index];
        const uint32_t elapsed_ms = now_ms - slot->previous_ms;
        if (elapsed_ms < slot->period_ms) {
            continue;
        }

        const can_driver_status_t status = can_driver_send_now(driver, &slot->frame);
        driver->next_periodic = (uint16_t)((index + 1U) % count);
        if (status != CAN_DRIVER_OK) {
            return status;
        }
        /* Skip missed periods rather than replaying old actuator demands. */
        slot->previous_ms = now_ms - (elapsed_ms % slot->period_ms);
        submitted++;
        if (driver->config.tx_budget != 0U && submitted >= driver->config.tx_budget) {
            break;
        }
    }
    return CAN_DRIVER_OK;
}

can_driver_status_t can_driver_update_periodic(can_driver_t *driver,
    uint16_t index, const can_driver_frame_t *frame)
{
    if (driver == NULL || !can_driver_frame_valid(frame)) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    if (index >= driver->config.periodic_count) {
        return CAN_DRIVER_INVALID;
    }
    can_driver_periodic_t *slot = &driver->config.periodic[index];
    if (slot->frame.id != frame->id || slot->frame.extended != frame->extended) {
        return CAN_DRIVER_INVALID;
    }
    slot->frame = *frame;
    return CAN_DRIVER_OK;
}

can_driver_status_t can_driver_receive_isr(can_driver_t *driver,
    const can_driver_frame_t *frame, uint32_t received_ms)
{
    if (driver == NULL) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    if (!can_driver_frame_valid(frame) || driver->config.rx_capacity == 0U) {
        atomic_fetch_add_explicit(&driver->rx_invalid, 1U, memory_order_relaxed);
        return CAN_DRIVER_INVALID;
    }
    const uint32_t head = atomic_load_explicit(&driver->rx_head, memory_order_relaxed);
    const uint32_t next = (head + 1U) % driver->config.rx_capacity;
    if (next == atomic_load_explicit(&driver->rx_tail, memory_order_acquire)) {
        atomic_fetch_add_explicit(&driver->rx_dropped, 1U, memory_order_relaxed);
        return CAN_DRIVER_BUSY;
    }

    driver->config.rx_storage[head].frame = *frame;
    driver->config.rx_storage[head].received_ms = received_ms;
    atomic_store_explicit(&driver->rx_head, next, memory_order_release);
    atomic_fetch_add_explicit(&driver->rx_received, 1U, memory_order_relaxed);
    return CAN_DRIVER_OK;
}

can_driver_status_t can_driver_receive(can_driver_t *driver,
    can_driver_rx_frame_t *out_frame)
{
    if (driver == NULL || out_frame == NULL) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    if (driver->config.rx_capacity == 0U) {
        return CAN_DRIVER_EMPTY;
    }
    const uint32_t tail = atomic_load_explicit(&driver->rx_tail, memory_order_relaxed);
    if (tail == atomic_load_explicit(&driver->rx_head, memory_order_acquire)) {
        return CAN_DRIVER_EMPTY;
    }

    *out_frame = driver->config.rx_storage[tail];
    atomic_store_explicit(&driver->rx_tail,
        (tail + 1U) % driver->config.rx_capacity, memory_order_release);
    return CAN_DRIVER_OK;
}

can_driver_status_t can_driver_get_stats(const can_driver_t *driver,
    can_driver_stats_t *out_stats)
{
    if (driver == NULL || out_stats == NULL) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    *out_stats = driver->tx_stats;
    out_stats->rx_received = atomic_load_explicit(&driver->rx_received, memory_order_relaxed);
    out_stats->rx_dropped = atomic_load_explicit(&driver->rx_dropped, memory_order_relaxed);
    out_stats->rx_invalid = atomic_load_explicit(&driver->rx_invalid, memory_order_relaxed);
    return CAN_DRIVER_OK;
}
