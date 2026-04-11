#include "generic_can_driver.h"
#include <stdint.h>
#include <string.h>

/*
    * HAL DLC to payload mapping
*/
#define FDCAN_DLC_BYTES_0  ((uint32_t)0x00000000U) /*!< 0 bytes data field  */
#define FDCAN_DLC_BYTES_1  ((uint32_t)0x00000001U) /*!< 1 bytes data field  */
#define FDCAN_DLC_BYTES_2  ((uint32_t)0x00000002U) /*!< 2 bytes data field  */
#define FDCAN_DLC_BYTES_3  ((uint32_t)0x00000003U) /*!< 3 bytes data field  */
#define FDCAN_DLC_BYTES_4  ((uint32_t)0x00000004U) /*!< 4 bytes data field  */
#define FDCAN_DLC_BYTES_5  ((uint32_t)0x00000005U) /*!< 5 bytes data field  */
#define FDCAN_DLC_BYTES_6  ((uint32_t)0x00000006U) /*!< 6 bytes data field  */
#define FDCAN_DLC_BYTES_7  ((uint32_t)0x00000007U) /*!< 7 bytes data field  */
#define FDCAN_DLC_BYTES_8  ((uint32_t)0x00000008U) /*!< 8 bytes data field  */
#define FDCAN_DLC_BYTES_12 ((uint32_t)0x00000009U) /*!< 12 bytes data field */
#define FDCAN_DLC_BYTES_16 ((uint32_t)0x0000000AU) /*!< 16 bytes data field */
#define FDCAN_DLC_BYTES_20 ((uint32_t)0x0000000BU) /*!< 20 bytes data field */
#define FDCAN_DLC_BYTES_24 ((uint32_t)0x0000000CU) /*!< 24 bytes data field */
#define FDCAN_DLC_BYTES_32 ((uint32_t)0x0000000DU) /*!< 32 bytes data field */
#define FDCAN_DLC_BYTES_48 ((uint32_t)0x0000000EU) /*!< 48 bytes data field */
#define FDCAN_DLC_BYTES_64 ((uint32_t)0x0000000FU) /*!< 64 bytes data field */

uint32_t CAN_payload_size_to_dlc(uint8_t payload_size){
    switch(payload_size){
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        case 8:
        default:
            return FDCAN_DLC_BYTES_8;
        case 12: return FDCAN_DLC_BYTES_12;
        case 16: return FDCAN_DLC_BYTES_16;
        case 20: return FDCAN_DLC_BYTES_20;
        case 24: return FDCAN_DLC_BYTES_24;
        case 32: return FDCAN_DLC_BYTES_32;
        case 48: return FDCAN_DLC_BYTES_48;
        case 64: return FDCAN_DLC_BYTES_64;
    }
}

uint8_t CAN_dlc_to_payload_size(uint32_t dlc){
    switch(dlc){
        case FDCAN_DLC_BYTES_0: return 0U;
        case FDCAN_DLC_BYTES_1: return 1U;
        case FDCAN_DLC_BYTES_2: return 2U;
        case FDCAN_DLC_BYTES_3: return 3U;
        case FDCAN_DLC_BYTES_4: return 4U;
        case FDCAN_DLC_BYTES_5: return 5U;
        case FDCAN_DLC_BYTES_6: return 6U;
        case FDCAN_DLC_BYTES_7: return 7U;
        case FDCAN_DLC_BYTES_8:
        default:
            return 8U;
        case FDCAN_DLC_BYTES_12: return 12U;
        case FDCAN_DLC_BYTES_16: return 16U;
        case FDCAN_DLC_BYTES_20: return 20U;
        case FDCAN_DLC_BYTES_24: return 24U;
        case FDCAN_DLC_BYTES_32: return 32U;
        case FDCAN_DLC_BYTES_48: return 48U;
        case FDCAN_DLC_BYTES_64: return 64U;
    }
}

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

/**
 * @brief Internal helper: Flushes pending TX frames into hardware FIFO.
 * @param max_frames_to_send 0 to send until FIFO full/queue empty, otherwise send at most this many.
 */
static void CAN_flush_tx_ring_buffer(CAN_Driver_t* driver, uint16_t max_frames_to_send) {
    uint16_t sent_frames = 0;

    if (driver == NULL ||
        driver->tx_ring_buffer.frame == NULL ||
        driver->tx_ring_buffer.size == 0 ||
        driver->add_to_fifo_fn == NULL ||
        driver->get_tx_fifo_free_level_fn == NULL) {
        return;
    }

    while(driver->tx_ring_buffer.head != driver->tx_ring_buffer.tail) {
        if (max_frames_to_send != 0U && sent_frames >= max_frames_to_send) {
            break;
        }

        if(driver->get_tx_fifo_free_level_fn(driver->hfdcan) == 0) {
            break;
        }

        if(driver->add_to_fifo_fn(driver->hfdcan,
            driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head].hdr,
            driver->tx_ring_buffer.frame[driver->tx_ring_buffer.head].payload) == 0) {
            driver->tx_ring_buffer.head = (driver->tx_ring_buffer.head + 1) % driver->tx_ring_buffer.size;
            sent_frames++;
        } else {
            break;
        }
    }
}

void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick) {
    uint8_t queue_was_empty;

    if (driver == NULL ||
        driver->tx_frame_configs == NULL ||
        driver->message_frames_tx == NULL ||
        driver->tx_ring_buffer.frame == NULL ||
        driver->tx_ring_buffer.size == 0 ||
        driver->add_to_fifo_fn == NULL ||
        driver->get_tx_fifo_free_level_fn == NULL) {
        return;
    }

    queue_was_empty = (driver->tx_ring_buffer.head == driver->tx_ring_buffer.tail);

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

    // Kickstart transmission once when queue transitions from empty to non-empty.
    // The TX FIFO empty interrupt callback continues draining after this.
    if (queue_was_empty && (driver->tx_ring_buffer.head != driver->tx_ring_buffer.tail)) {
        CAN_flush_tx_ring_buffer(driver, 1U);
    }
}

void CAN_driver_tx_fifo_empty_callback(CAN_Driver_t* driver) {
    CAN_flush_tx_ring_buffer(driver, 0U);
}

uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame) {
    return driver->add_to_fifo_fn(driver->hfdcan, frame->hdr, frame->payload);
}