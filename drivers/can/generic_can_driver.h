#ifndef INC_GENERIC_CAN_DRIVER_H
#define INC_GENERIC_CAN_DRIVER_H

#include <stdint.h>

#define CAN_DRIVER_NON_PERIODIC_FRAME 0xFFFF

/**
 * @brief Function pointer for transmitting CAN messages.
 */
typedef uint32_t (*CanTxFn_t)(void *hfdcan, void *hdr, uint8_t *payload);

/**
 * @brief Function pointer for checking CAN FIFO free level
 */
typedef uint32_t (*CanFifoFreeLevelFn_t)(void *hfdcan);

/**
 * @brief Structure representing a CAN message.
 */
typedef struct {
    void* hdr;
    uint8_t payload[8];
    uint32_t msg_id;
    uint8_t num_values;
} CAN_Tx_Message_Frame_t;

/**
 * @brief Structure representing a received CAN message.
 */
typedef struct {
    uint8_t payload[8];
    uint32_t msg_id;
    uint8_t num_values;
    uint32_t rx_tick_ms;
} CAN_Rx_Message_Frame_t;

/**
 * @brief Circular buffer for storing received CAN messages.
 */
typedef struct {
    CAN_Rx_Message_Frame_t* frame;
    uint16_t size; // Size of the ring buffer (number of frames it can hold)
    volatile uint16_t head; // Index for the next incoming frame
    volatile uint16_t tail; // Index for the next frame to process
} CAN_Rx_Ring_Buffer_t;

/**
 * @brief Circular buffer for storing pending TX CAN messages.
 */
typedef struct {
    CAN_Tx_Message_Frame_t* frame;
    uint16_t size; // Size of the ring buffer (number of frames it can hold)
    volatile uint16_t head; // Index for the next frame to transmit
    volatile uint16_t tail; // Index for the next slot to enqueue
} CAN_Tx_Ring_Buffer_t;

/**
 * @brief Configuration settings for a TX CAN frame and its scheduler state.
 */
typedef struct {
    uint32_t payload_size;
    uint32_t id;
    uint32_t scheduler_timer_value;
    uint32_t scheduler_timer_prev_tick;
} CAN_Tx_Timer_t;

/**
 * @brief Main driver structure holding state and buffers.
 */
typedef struct {
    CAN_Tx_Message_Frame_t* message_frames_tx;

    CAN_Rx_Ring_Buffer_t rx_ring_buffer;
    CAN_Tx_Ring_Buffer_t tx_ring_buffer;

    uint8_t tx_frame_number;
    uint8_t tx_ring_buffer_size;
    uint8_t rx_ring_buffer_size;

    void* hfdcan;

    // Array to hold configurations for each CAN frame to be transmitted + scheduler state
    CAN_Tx_Timer_t* tx_frame_configs;

    volatile uint8_t can_new_message_flag; // Flag to indicate a new message has been received

    CanTxFn_t add_to_fifo_fn; // Function pointer for adding messages to the CAN Tx FIFO
    CanFifoFreeLevelFn_t get_tx_fifo_free_level_fn; // Function pointer for checking CAN FIFO free level
} CAN_Driver_t;



/**
 * @brief Initializes the driver with hardware handle and TX function.
 * @param driver Driver instance.
 * @param add_to_fifo_fn TX function pointer.
 * @param get_tx_fifo_free_level_fn Function pointer for checking CAN FIFO free level.
 * @param hfdcan_instance Hardware handle.
 */
void CAN_set_structures(CAN_Driver_t* driver, 
    CanTxFn_t add_to_fifo_fn, 
    CanFifoFreeLevelFn_t get_tx_fifo_free_level_fn, 
    void* hfdcan_instance);

/**
 * @brief Callback for RX interrupts to store data in ring buffer.
 * @param driver Driver instance.
 * @param data Payload data.
 * @param msg_id Message ID.
 */
void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, uint32_t msg_id, uint8_t num_values, uint32_t rx_tick_ms);

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
