---
title: Common - CAN
description: Generated CAN definitions and the generic CAN driver.
---

# CAN

## Definitions (generated)
Generated from MAIN_DBC.dbc by dbc_to_c_builder.py.

- can_signal_defs.h provides per-message structs, message IDs, DLC, and factor macros.
- can_board_defs.h groups transmit messages by sender board.

> [!NOTE]
> Do not edit the generated headers by hand.

### Usage
```c
#include "can_signal_defs.h"
#include "can_board_defs.h"

can_msg_GF_Wheel_Sensors_t gf = {0};
can_board_ECU_t ecu = {0};

gf.Wheel_Speed_FR = 1200;
ecu.ECU_Inverter_Sig.Calculated_Speed = 42.0f;
```

## Generic CAN Driver (generic_can_driver.h)
Ring-buffered CAN TX scheduling and RX queueing with user-supplied HAL callbacks. The driver keeps only the newest queued frame per CAN ID and rotates the scheduler start index to avoid fixed-priority bias.

### Functions
#### `void CAN_set_structures(CAN_Driver_t* driver, CanTxFn_t add_to_fifo_fn, CanTxFifoLevelFn_t get_tx_fifo_level_fn, void* hfdcan_instance)`
Initializes the driver with the hardware handle and callback functions.

#### `void CAN_driver_rx_callback(CAN_Driver_t* driver, uint8_t* data, void* hdr_rx, uint32_t msg_id, uint8_t num_values, uint32_t timestamp)`
ISR-safe RX callback that writes a received frame into the RX ring buffer.

#### `void CAN_send_frames(CAN_Driver_t* driver, uint32_t current_tick)`
Schedules periodic frames based on per-frame scheduler timers.

#### `uint16_t CAN_process_tx_queue(CAN_Driver_t* driver, uint16_t amount)`
Drains queued TX frames into the hardware FIFO while space is available.

#### `uint32_t CAN_send_single_frame(CAN_Driver_t* driver, CAN_Tx_Message_Frame_t* frame)`
Queues a single frame for transmission. Returns 0 on success, 1 on failure.

### Usage
```c
#include "generic_can_driver.h"

static CAN_Tx_Message_Frame_t scheduled_frames[4];
static CAN_Tx_Message_Frame_t tx_queue_frames[8];
static CAN_Rx_Message_Frame_t rx_frames[8];

static CAN_Driver_t can_driver = {
    .tx_message_frames = scheduled_frames,
    .tx_frame_number = 4,
    .rx_frame_number = 8,
    .tx_ring_buffer = { .frame = tx_queue_frames, .size = 8 },
    .rx_ring_buffer = { .frame = rx_frames, .size = 8 },
};

static uint32_t can_add_to_fifo(void* hfdcan, void* hdr, uint8_t* payload);
static uint32_t can_get_fifo_level(void* hfdcan);

void can_init(void* hfdcan_instance) {
    CAN_set_structures(&can_driver, can_add_to_fifo, can_get_fifo_level, hfdcan_instance);
}

void can_rx_isr(uint8_t* data, void* rx_hdr, uint32_t msg_id, uint8_t len, uint32_t tick) {
    CAN_driver_rx_callback(&can_driver, data, rx_hdr, msg_id, len, tick);
}

void can_task(uint32_t now) {
    CAN_send_frames(&can_driver, now);
    if (can_driver.tx_queue_drain_requested) {
        (void)CAN_process_tx_queue(&can_driver, 0);
    }
}
```

> [!WARNING]
> `CAN_driver_rx_callback` is intended for ISR context; do not do heavy work inside it.
