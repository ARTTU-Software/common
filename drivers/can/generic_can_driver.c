#include "generic_can_driver.h"
#include <stdint.h>
#include <string.h>

volatile uint32_t can_dbg_tx_enqueue_full_count = 0U;
volatile uint32_t can_dbg_tx_id_replace_count = 0U;
volatile uint32_t can_dbg_tx_processed_count = 0U;
volatile uint16_t can_dbg_tx_queue_high_watermark = 0U;
volatile uint32_t can_dbg_tx_catchup_count = 0U;
volatile uint32_t can_dbg_tx_phase_resync_count = 0U;
static volatile uint8_t can_tx_drain_in_progress = 0U;

#define CAN_TX_MAX_CATCHUP_PER_CALL 2U

static uint32_t CAN_enqueue_tx_frame(CAN_Driver_t* driver, const CAN_Tx_Message_Frame_t* frame)
{
    uint16_t offset;

    if (driver == NULL || frame == NULL || driver->tx_ring_buffer.frame == NULL || driver->tx_ring_buffer.size == 0U) {
        return 1U;
    }

    // Keep only the freshest queued frame per CAN ID.
    for (offset = 0U; offset < driver->tx_ring_buffer.count; offset++) {
        uint16_t index = (uint16_t)((driver->tx_ring_buffer.tail + offset) % driver->tx_ring_buffer.size);

        if (driver->tx_ring_buffer.frame[index].msg_id == frame->msg_id) {
            driver->tx_ring_buffer.frame[index] = *frame;
            can_dbg_tx_id_replace_count++;
            driver->tx_queue_drain_requested = 1;
            return 0U;
        }
    }

    if (driver->tx_ring_buffer.count == driver->tx_ring_buffer.size) {
        can_dbg_tx_enqueue_full_count++;
        return 1U;
    }

    driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head] = *frame;
    driver->tx_ring_buffer.head = (uint16_t)((driver->tx_ring_buffer.head + 1U) % driver->tx_ring_buffer.size);
    driver->tx_ring_buffer.count++;
    if (driver->tx_ring_buffer.count > can_dbg_tx_queue_high_watermark) {
        can_dbg_tx_queue_high_watermark = driver->tx_ring_buffer.count;
    }
    driver->tx_queue_drain_requested = 1;

    return 0U;
}

void CAN_set_structures(CAN_Driver_t* driver, 
    CanTxFn_t add_to_fifo_fn,
    CanTxFifoLevelFn_t get_tx_fifo_level_fn, 
    void* hfdcan_instance)
{
    driver->add_to_fifo_fn = add_to_fifo_fn;
    driver->get_tx_fifo_level_fn = get_tx_fifo_level_fn;

    driver->hfdcan = hfdcan_instance;
    driver->can_new_message_flag = 0;
    driver->tx_queue_drain_requested = 0;
    driver->tx_scheduler_start_index = 0;

    // Init ring buffer values
    driver->tx_ring_buffer.head = 0;
    driver->tx_ring_buffer.tail = 0;
    driver->tx_ring_buffer.count = 0;
    driver->rx_ring_buffer.size = driver->rx_frame_number;
    driver->rx_ring_buffer.head = 0;
    driver->rx_ring_buffer.tail = 0;

    for(uint32_t i = 0; i < driver->tx_frame_number; i++){
        driver->tx_message_frames[i].previous_tick = 0;
    }
}

void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, void* hdr_rx, uint32_t msg_id, uint8_t num_values, uint32_t timestamp) {
    uint16_t write_index;

    if (driver == NULL || driver->rx_ring_buffer.frame == NULL || driver->rx_ring_buffer.size == 0)
        return;

    // overflow check
    uint16_t next_head = (driver->rx_ring_buffer.head + 1) % driver->rx_ring_buffer.size;

    if (next_head == driver->rx_ring_buffer.tail) {
        // buffer is full, so drop message
        return; 
    }

    // Uses the ring buffer in the driver structure to store incoming messages
    write_index = driver->rx_ring_buffer.head;
    memcpy(driver->rx_ring_buffer.frame[write_index].payload, data, num_values);
    driver->rx_ring_buffer.frame[write_index].msg_id = msg_id;
    driver->rx_ring_buffer.frame[write_index].num_values = num_values;
    driver->rx_ring_buffer.frame[write_index].previous_tick = timestamp;

    // Increment head in ring buffer
    driver->rx_ring_buffer.head = (driver->rx_ring_buffer.head + 1) % driver->rx_ring_buffer.size;

    // Set flag to indicate a new message has been received
    driver->can_new_message_flag = 1;
}

void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick) {
    uint8_t attempts;
    uint16_t index;

    if(driver == NULL || driver->tx_message_frames == NULL)
        return;

    for(uint16_t i = 0; i < driver->tx_frame_number; i++){
        index = (uint16_t)((driver->tx_scheduler_start_index + i) % driver->tx_frame_number);

        if(driver->tx_message_frames[index].scheduler_timer_value == CAN_DRIVER_NON_PERIODIC_FRAME){
            continue; // Skip non-periodic frames
        }
        attempts = 0U;
        while ((current_tick - driver->tx_message_frames[index].previous_tick >= driver->tx_message_frames[index].scheduler_timer_value) &&
               (attempts < CAN_TX_MAX_CATCHUP_PER_CALL)) {
            if (CAN_enqueue_tx_frame(driver, &driver->tx_message_frames[index]) == 0U) {
                driver->tx_message_frames[index].previous_tick += driver->tx_message_frames[index].scheduler_timer_value;
                attempts++;
            } else {
                break;
            }
        }

        if (attempts > 1U) {
            can_dbg_tx_catchup_count += (uint32_t)(attempts - 1U);
        }

        // Prevent very old phase from accumulating indefinitely after long stalls.
        if (current_tick - driver->tx_message_frames[index].previous_tick >= driver->tx_message_frames[index].scheduler_timer_value) {
            driver->tx_message_frames[index].previous_tick = current_tick - driver->tx_message_frames[index].scheduler_timer_value;
            can_dbg_tx_phase_resync_count++;
        }
    }

    if (driver->tx_frame_number > 0U) {
        driver->tx_scheduler_start_index = (uint8_t)((driver->tx_scheduler_start_index + 1U) % driver->tx_frame_number);
    }
}

uint16_t CAN_process_tx_queue(CAN_Driver_t* driver, uint16_t amount)
{
    uint16_t processed_count = 0U;
    uint16_t max_to_process;

    if (driver == NULL || driver->tx_ring_buffer.frame == NULL || driver->tx_ring_buffer.size == 0U) {
        return 0U;
    }

    if (driver->add_to_fifo_fn == NULL) {
        return 0U;
    }

    if (can_tx_drain_in_progress != 0U) {
        return 0U;
    }

    can_tx_drain_in_progress = 1U;

    max_to_process = (amount == 0U || amount > driver->tx_ring_buffer.count) ? driver->tx_ring_buffer.count : amount;

    while (driver->tx_ring_buffer.count > 0U && processed_count < max_to_process) {
        CAN_Tx_Message_Frame_t* queued_frame = &driver->tx_ring_buffer.frame[driver->tx_ring_buffer.tail];

        if (driver->get_tx_fifo_level_fn != NULL) {
            uint32_t tx_fifo_free_level = driver->get_tx_fifo_level_fn(driver->hfdcan);
            if (tx_fifo_free_level == 0U) {
                break;
            }
        }

        volatile uint32_t status = driver->add_to_fifo_fn(driver->hfdcan, queued_frame->hdr, queued_frame->payload);

        if (status != 0U) {
            break;
        }

        driver->tx_ring_buffer.tail = (uint16_t)((driver->tx_ring_buffer.tail + 1U) % driver->tx_ring_buffer.size);
        driver->tx_ring_buffer.count--;
        processed_count++;
        can_dbg_tx_processed_count++;
    }

    driver->tx_queue_drain_requested = (driver->tx_ring_buffer.count > 0U) ? 1U : 0U;
    can_tx_drain_in_progress = 0U;

    return processed_count;
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    return CAN_enqueue_tx_frame(driver, frame);
}