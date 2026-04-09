#include "generic_can_driver.h"
#include <stdint.h>
#include <string.h>

void CAN_set_structures(CAN_Driver_t* driver, 
    CanTxFn_t add_to_fifo_fn, 
    CanFifoFreeLevelFn_t get_tx_fifo_free_level_fn,
    void* hfdcan_instance)
{
    driver->add_to_fifo_fn = add_to_fifo_fn;
    driver->get_tx_fifo_free_level_fn = get_tx_fifo_free_level_fn;
    driver->hfdcan = hfdcan_instance;
    driver->can_new_message_flag = 0;

    // Init RX ring buffer values
    driver->rx_ring_buffer.size = driver->rx_ring_buffer_size;
    driver->rx_ring_buffer.head = 0;
    driver->rx_ring_buffer.tail = 0;

    // Init TX ring buffer values
    driver->tx_ring_buffer.size = driver->tx_ring_buffer_size;
    driver->tx_ring_buffer.head = 0;
    driver->tx_ring_buffer.tail = 0;

    for(uint32_t i = 0; i < driver->tx_frame_number; i++){
        driver->message_frames_tx[i].msg_id = driver->tx_frame_configs[i].id;
        driver->message_frames_tx[i].num_values = driver->tx_frame_configs[i].payload_size;
    }
}

void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, uint32_t msg_id, uint8_t num_values, uint32_t rx_tick_ms) {
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
    driver->rx_ring_buffer.frame[driver->rx_ring_buffer.head].rx_tick_ms = rx_tick_ms;

    driver->rx_ring_buffer.head = next_head;
    driver->can_new_message_flag = 1;
}

/**
 * @brief Internal helper: Enqueues a TX frame to the ring buffer.
 * @return 1 if successfully enqueued, 0 if buffer is full.
 */
static uint32_t CAN_enqueue_tx_frame(CAN_Driver_t* driver, const CAN_Tx_Message_Frame_t* frame) {
    if (driver == NULL || driver->tx_ring_buffer.frame == NULL || driver->tx_ring_buffer.size == 0)
        return 0;

    uint16_t next_tail = (driver->tx_ring_buffer.tail + 1) % driver->tx_ring_buffer.size;

    // Overflow check: buffer is full
    if (next_tail == driver->tx_ring_buffer.head) {
        return 0;
    }

    // Copy frame to buffer
    driver->tx_ring_buffer.frame[driver->tx_ring_buffer.tail] = *frame;
    driver->tx_ring_buffer.tail = next_tail;

    return 1;
}

void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick) {
    if (driver == NULL ||
        driver->tx_frame_configs == NULL ||
        driver->message_frames_tx == NULL ||
        driver->tx_ring_buffer.frame == NULL ||
        driver->tx_ring_buffer.size == 0 ||
        driver->add_to_fifo_fn == NULL ||
        driver->get_tx_fifo_free_level_fn == NULL) {
        return;
    }

    // enqueue periodic frames that are due to the TX ring buffer
    for(uint16_t i = 0; i < driver->tx_frame_number; i++){
        
        if(driver->tx_frame_configs[i].scheduler_timer_value == CAN_DRIVER_NON_PERIODIC_FRAME) {
            continue; // skip non periodic frames
        }
        
        if(current_tick - driver->tx_frame_configs[i].scheduler_timer_prev_tick >= driver->tx_frame_configs[i].scheduler_timer_value){
            // Enqueue frame to TX buffer; if buffer is full, skip and retry next cycle
            if(CAN_enqueue_tx_frame(driver, &driver->message_frames_tx[i])) {
                driver->tx_frame_configs[i].scheduler_timer_prev_tick = current_tick;
            }
        }
    }

    // flush TX ring buffer to hardware FIFO
    // pull frames until either buffer is empty or hardware FIFO is full
    while(driver->tx_ring_buffer.head != driver->tx_ring_buffer.tail) {
        if(driver->get_tx_fifo_free_level_fn(driver->hfdcan) == 0) {
            break;  // resume sending next cycle if hardware FIFO is full
        }

        if(driver->add_to_fifo_fn(driver->hfdcan,
            driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head].hdr,
            driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head].payload) == 0) {
            
            driver->tx_ring_buffer.head = (driver->tx_ring_buffer.head + 1) % driver->tx_ring_buffer.size;
        } else {
            // hardware rejected frame; stop attempting to send this cycle
            break;
        }
    }
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    return driver->add_to_fifo_fn(driver->hfdcan, frame->hdr, frame->payload);
}