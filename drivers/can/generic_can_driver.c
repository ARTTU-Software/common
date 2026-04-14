#include "generic_can_driver.h"
#include <stdint.h>
#include <string.h>

static void CAN_enqueue_tx_frame(CAN_Driver_t* driver, const CAN_Tx_Message_Frame_t* frame)
{
    if (driver == NULL || frame == NULL || driver->tx_ring_buffer.frame == NULL || driver->tx_ring_buffer.size == 0U) {
        return;
    }

    if (driver->tx_ring_buffer.count == driver->tx_ring_buffer.size) {
        driver->tx_ring_buffer.tail = (uint16_t)((driver->tx_ring_buffer.tail + 1U) % driver->tx_ring_buffer.size);
        driver->tx_ring_buffer.count--;
    }

    driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head] = *frame;
    driver->tx_ring_buffer.head = (uint16_t)((driver->tx_ring_buffer.head + 1U) % driver->tx_ring_buffer.size);
    driver->tx_ring_buffer.count++;
    driver->tx_queue_drain_requested = 1;
}

void CAN_set_structures(CAN_Driver_t* driver, 
    CanTxFn_t add_to_fifo_fn, 
    void* hfdcan_instance)
{
    driver->add_to_fifo_fn = add_to_fifo_fn;
    driver->hfdcan = hfdcan_instance;
    driver->can_new_message_flag = 0;
    driver->tx_queue_drain_requested = 0;

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
    if (driver == NULL || driver->rx_ring_buffer.frame == NULL || driver->rx_ring_buffer.size == 0)
        return;

    // overflow check
    uint16_t next_head = (driver->rx_ring_buffer.head + 1) % driver->rx_ring_buffer.size;

    if (next_head == driver->rx_ring_buffer.tail) {
        // buffer is full, so drop message
        return; 
    }

    // Uses the ring buffer in the driver structure to store incoming messages
    memcpy(driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].payload, data, num_values);
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].msg_id = msg_id;
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].num_values = num_values;

    // Increment head in ring buffer
    driver->rx_ring_buffer.head = (driver->rx_ring_buffer.head + 1) % driver->rx_ring_buffer.size;

    // Set timestamp
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].previous_tick = timestamp;

    // Set flag to indicate a new message has been received
    driver->can_new_message_flag = 1;
}

void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick) {
    if(driver == NULL || driver->tx_message_frames == NULL)
        return;

    for(uint16_t i = 0; i < driver->tx_frame_number; i++){
        if(driver->tx_message_frames[i].scheduler_timer_value == CAN_DRIVER_NON_PERIODIC_FRAME){
            continue; // Skip non-periodic frames
        }
        if(current_tick - driver->tx_message_frames[i].previous_tick >= driver->tx_message_frames[i].scheduler_timer_value){
            driver->tx_message_frames[i].previous_tick = current_tick;
            CAN_enqueue_tx_frame(driver, &driver->tx_message_frames[i]);
        }
    }
}

void CAN_process_tx_queue(CAN_Driver_t* driver, uint32_t ammount_to_process)
{
    if (driver == NULL || driver->tx_ring_buffer.frame == NULL || driver->tx_ring_buffer.size == 0U) {
        return;
    }

    uint8_t processed = 0;
    if(ammount_to_process == 0U){
        ammount_to_process = driver->tx_ring_buffer.count; // Process all if ammount_to_process is 0
    }
    while (driver->tx_ring_buffer.count > 0U && processed < ammount_to_process) {
        CAN_Tx_Message_Frame_t* queued_frame = &driver->tx_ring_buffer.frame[driver->tx_ring_buffer.tail];
        uint32_t status = driver->add_to_fifo_fn(driver->hfdcan, queued_frame->hdr, queued_frame->payload);

        if (status != 0U) {
            break;
        }

        driver->tx_ring_buffer.tail = (uint16_t)((driver->tx_ring_buffer.tail + 1U) % driver->tx_ring_buffer.size);
        driver->tx_ring_buffer.count--;
        processed++;
    }

    driver->tx_queue_drain_requested = (driver->tx_ring_buffer.count > 0U) ? 1U : 0U;
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    CAN_enqueue_tx_frame(driver, frame);
    return 0U;
}