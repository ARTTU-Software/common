#include "generic_can_driver.h"
#include <stdint.h>
#include <string.h>

void CAN_set_structures(CAN_Driver_t* driver, 
    CanTxFn_t add_to_fifo_fn, 
    void* hfdcan_instance)
{
    driver->add_to_fifo_fn = add_to_fifo_fn;
    driver->hfdcan = hfdcan_instance;
    driver->can_new_message_flag = 0;

    // Init ring buffer values
    driver->rx_ring_buffer.size = driver->rx_frame_number;
    driver->rx_ring_buffer.head = 0;
    driver->rx_ring_buffer.tail = 0;

    for(uint32_t i = 0; i < driver->tx_frame_number; i++){
        driver->tx_message_frames[i].previous_tick = 0;
    }
}

void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, void* hdr_rx, uint32_t msg_id, uint8_t num_values, uint32_t timestamp) {
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
    for(uint16_t i = 0; i < driver->tx_frame_number; i++){
        if(driver->tx_message_frames[i].scheduler_timer_value == CAN_DRIVER_NON_PERIODIC_FRAME){
            continue; // Skip non-periodic frames
        }
        if(current_tick - driver->tx_message_frames[i].previous_tick >= driver->tx_message_frames[i].scheduler_timer_value){
            driver->tx_message_frames[i].previous_tick = current_tick;
            driver->add_to_fifo_fn(driver->hfdcan, driver->tx_message_frames[i].hdr, driver->tx_message_frames[i].payload);
        }
    }
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    return driver->add_to_fifo_fn(driver->hfdcan, frame->hdr, frame->payload);
}