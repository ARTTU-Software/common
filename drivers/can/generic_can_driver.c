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
        driver->message_frames_tx[i].msg_id = driver->tx_frame_configs[i].id;
        driver->message_frames_tx[i].num_values = driver->tx_frame_configs[i].payload_size;
    }
}

void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, uint32_t msg_id, uint8_t num_values) {
    if (driver == NULL || driver->rx_ring_buffer.frame == NULL || driver->rx_ring_buffer.size == 0)
        return;

    uint16_t next_head = (driver->rx_ring_buffer.head + 1) % driver->rx_ring_buffer.size;

    // overflow check
    if (next_head == driver->rx_ring_buffer.tail) {
        // buffer is full, so drop message
        return; 
    }

    // drop invalid length messages
    if (num_values > (uint8_t)sizeof(driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].payload)) {
        return;
    }
    uint8_t rx_len_bytes = num_values;

    // always clear payload so missing bytes cannot leak stale values.
    memset(driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].payload, 0, sizeof(driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].payload));
    memcpy(driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].payload, data, rx_len_bytes);
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].msg_id = msg_id;
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].num_values = rx_len_bytes;

    driver->rx_ring_buffer.head = next_head;
    driver->can_new_message_flag = 1;
}

void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick) {
    for(uint16_t i = 0; i < driver->tx_frame_number; i++){
        if(current_tick - driver->tx_scheduler_prev_tick[i] >= driver->tx_frame_configs[i].scheduler_timer_value){
            driver->tx_scheduler_prev_tick[i] = current_tick;
            driver->add_to_fifo_fn(driver->hfdcan, 
                driver->message_frames_tx[i].hdr, 
                driver->message_frames_tx[i].payload);
        }
    }
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    return driver->add_to_fifo_fn(driver->hfdcan, frame->hdr, frame->payload);
}