#ifndef INC_GENERIC_CAN_DRIVER_H
#define INC_GENERIC_CAN_DRIVER_H

#include <stdint.h>

#define CAN_DRIVER_NON_PERIODIC_FRAME 0xFFFF

/**
 * @brief Function pointer for transmitting CAN messages.
 */
typedef uint32_t (*CanTxFn_t)(void *hfdcan, void *hdr, uint8_t *payload);

/**
 * @brief Structure representing a CAN message.
 */
typedef struct {
    void* hdr;
    uint8_t payload[8];
    uint32_t msg_id;
    uint8_t num_values;
    uint32_t previous_tick; // For timestamping or scheduling purposes
    uint32_t scheduler_timer_value; // For periodic transmission scheduling
} CAN_Tx_Message_Frame_t;

typedef struct {
    uint8_t payload[8];
    uint32_t msg_id;
    uint8_t num_values;
    uint32_t previous_tick; // For timestamping or scheduling purposes
} CAN_Rx_Message_Frame_t;

/**
 * @brief Circular buffer for storing received CAN messages.
 */
typedef struct {
    CAN_Rx_Message_Frame_t* frame;
    uint16_t size; // Size of the ring buffer (number of frames it can hold)
    uint16_t head; // Index for the next incoming frame
    uint16_t tail; // Index for the next frame to process
} CAN_Rx_Ring_Buffer_t;

/**
 * @brief Main driver structure holding state and buffers.
 */
typedef struct {
    CAN_Tx_Message_Frame_t* tx_message_frames;
    CAN_Rx_Ring_Buffer_t rx_ring_buffer;

    uint8_t tx_frame_number;
    uint8_t rx_frame_number;

    void* hfdcan;

    volatile uint8_t can_new_message_flag; // Flag to indicate a new message has been received

    CanTxFn_t add_to_fifo_fn; // Function pointer for adding messages to the CAN Tx FIFO
} CAN_Driver_t;

/**
 * @brief Initializes the driver with hardware handle and TX function.
 * @param driver Driver instance.
 * @param add_to_fifo_fn TX function pointer.
 * @param hfdcan_instance Hardware handle.
 */
void CAN_set_structures(CAN_Driver_t* driver, CanTxFn_t add_to_fifo_fn, void* hfdcan_instance);

/**
 * @brief Callback for RX interrupts to store data in ring buffer.
 * @param driver Driver instance.
 * @param data Payload data.
 * @param hdr_rx RX header.
 * @param msg_id Message ID.
 * @param timestamp Timestamp for the received message.
 */
void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, void* hdr_rx, uint32_t msg_id, uint8_t num_values, uint32_t timestamp);

/**
 * @brief Periodic task to send scheduled CAN frames.
 * @param driver Driver instance.
 * @param current_tick Current system time.
 */
void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick);

/**
 * @brief Sends a single CAN frame immediately.
 * @param driver Driver instance.
 * @param frame Frame to send.
 * @return Status code.
 */
uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame);

#endif /* INC_GENERIC_CAN_DRIVER_H */