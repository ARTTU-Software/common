---
title: CAN Integration Guide
description: Step-by-step guide to integrating the generic CAN driver on a new board.
---

# CAN Integration Guide

This guide details setting up the generic CAN driver on a board, using implementation details from the ECU project as a reference.

## File Structure

Board CAN implementation requires the following files:

```
Core/Inc/App/Communication/
├── can_driver.h    <- buffer sizes, externs, HAL callback prototypes
└── can_comm.h      <- RX/TX data structs, process/set function prototypes

Core/Src/App/Communication/
├── can_driver.c    <- buffer allocation, frame config, HAL callbacks, init
└── can_comm.c      <- RX message handler, TX payload packer
```

---

## Step 1: Define Buffer Sizes (`can_driver.h`)

Define frame slot counts and ring buffer dimensions:

```c
#include "generic_can_driver.h"

#define HZ_TO_MS(hz) (1000 / (hz))

#define NR_OF_CAN_TX_FRAMES        10   // Number of distinct TX messages this board sends
#define NR_OF_CAN_RX_BUFFER_FRAMES 128  // RX ring buffer depth
#define NR_OF_CAN_TX_BUFFER_FRAMES 64   // TX queue depth (software to HW FIFO)

extern CAN_Driver_t can_driver;

void init_canbus_driver(CAN_Driver_t *driver,
    CanTxFn_t add_to_fifo_fn,
    CanTxFifoLevelFn_t get_tx_fifo_level_fn,
    void *hfdcan_instance);
```

> [!TIP]
> **RX Buffer Sizing**: The buffer must accommodate all frames received between consecutive `process_can_frames()` execution cycles. 128 slots is recommended for standard bus traffic.

---

## Step 2: Allocate Buffers & Configure Frames (`can_driver.c`)

Allocate static arrays for messages and configure slot parameters (ID, DLC, transmission rate):

```c
#include "can_driver.h"
#include "generic_can_driver.h"

FDCAN_TxHeaderTypeDef tx_header_templates[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_message_frames[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_buffer_frames[NR_OF_CAN_TX_BUFFER_FRAMES];
CAN_Rx_Message_Frame_t rx_message_frame[NR_OF_CAN_RX_BUFFER_FRAMES];

CAN_Driver_t can_driver;
```

### Configuring TX Frame Slots

Assign properties per message slot:

```c
static void set_tx_configs(CAN_Driver_t* driver) {
    // Periodic frame: scheduled at 50 Hz (20 ms period)
    driver->tx_message_frames[0].msg_id = 0x0E0;
    driver->tx_message_frames[0].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[0].scheduler_timer_value = HZ_TO_MS(50);

    // Non-periodic frame: transmitted manually via CAN_send_single_frame()
    driver->tx_message_frames[1].msg_id = 0x0B3;
    driver->tx_message_frames[1].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[1].scheduler_timer_value = CAN_DRIVER_NON_PERIODIC_FRAME;
}
```

Use `HZ_TO_MS(freq)` for continuous broadcast data. Use `CAN_DRIVER_NON_PERIODIC_FRAME` for event-triggered transmissions.

### Driver Initialization Function

Bind allocated memory to the driver instance and populate peripheral header templates:

```c
void init_canbus_driver(CAN_Driver_t* driver,
    CanTxFn_t add_to_fifo_fn,
    CanTxFifoLevelFn_t get_tx_fifo_level_fn,
    void* hfdcan_instance)
{
    driver->rx_frame_number = NR_OF_CAN_RX_BUFFER_FRAMES;
    driver->tx_frame_number = NR_OF_CAN_TX_FRAMES;

    driver->tx_message_frames = tx_message_frames;
    driver->tx_ring_buffer.frame = tx_buffer_frames;
    driver->tx_ring_buffer.size = NR_OF_CAN_TX_BUFFER_FRAMES;
    driver->rx_ring_buffer.frame = rx_message_frame;

    set_tx_configs(driver);

    for (uint8_t i = 0; i < NR_OF_CAN_TX_FRAMES; i++) {
        tx_header_templates[i].IdType = FDCAN_STANDARD_ID;
        tx_header_templates[i].TxFrameType = FDCAN_DATA_FRAME;
        tx_header_templates[i].ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        tx_header_templates[i].BitRateSwitch = FDCAN_BRS_OFF;
        tx_header_templates[i].FDFormat = FDCAN_CLASSIC_CAN;
        tx_header_templates[i].TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        tx_header_templates[i].MessageMarker = 0;
        tx_header_templates[i].DataLength = driver->tx_message_frames[i].num_values;
        tx_header_templates[i].Identifier = driver->tx_message_frames[i].msg_id;

        driver->tx_message_frames[i].hdr = (void*)&tx_header_templates[i];
    }

    CAN_set_structures(driver, add_to_fifo_fn, get_tx_fifo_level_fn, hfdcan_instance);
}
```

---

## Step 3: HAL Interrupt Callbacks (`can_driver.c`)

Connect STM32 HAL interrupt callbacks to the driver interface.

### RX Interrupt Callback

Reads peripheral RX FIFO entries and pushes data to the software ring buffer:

```c
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0U) {
            FDCAN_RxHeaderTypeDef can_rx_hdr;
            uint8_t data[8];
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &can_rx_hdr, data) != HAL_OK)
                return;

            CAN_driver_rx_callback(&can_driver, data, (void*)&can_rx_hdr,
                can_rx_hdr.Identifier, (uint8_t)can_rx_hdr.DataLength, HAL_GetTick());
        }
    }
}
```

### TX Buffer Callbacks

Set drain request flags when peripheral hardware TX slots become available:

```c
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan) {
    if (hfdcan == (FDCAN_HandleTypeDef*)can_driver.hfdcan) {
        can_driver.tx_queue_drain_requested = 1;
    }
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes) {
    (void)BufferIndexes;
    if (hfdcan == (FDCAN_HandleTypeDef*)can_driver.hfdcan) {
        can_driver.tx_queue_drain_requested = 1;
    }
}
```

### Bus-Off Error Recovery

```c
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs) {
    if ((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0) {
        hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT;
    }
}
```

---

## Step 4: RX Processing Logic (`can_comm.c`)

Unpack incoming messages from the RX ring buffer in main context:

```c
// can_comm.h
typedef struct {
    can_msg_GF_Wheel_Sensors_t gf_wheel_sensors;
    can_msg_GR_Misc_Sensors_t  gr_misc_sensors;
} CAN_data_rx_t;

extern volatile CAN_data_rx_t can_rx_data;
```

```c
// can_comm.c
volatile CAN_data_rx_t can_rx_data;

void process_can_frames(CAN_Driver_t* driver) {
    while (driver->rx_ring_buffer.head != driver->rx_ring_buffer.tail) {
        CAN_Rx_Message_Frame_t current_frame =
            driver->rx_ring_buffer.frame[driver->rx_ring_buffer.tail];
        driver->rx_ring_buffer.tail =
            (driver->rx_ring_buffer.tail + 1) % driver->rx_ring_buffer.size;

        switch (current_frame.msg_id) {
            case 0x0A0: // GF Wheel Sensors
                can_rx_data.gf_wheel_sensors.Wheel_Speed_FR =
                    CAN_COMBINE_16(current_frame.payload[0], current_frame.payload[1]);
                can_rx_data.gf_wheel_sensors.Wheel_Speed_FL =
                    CAN_COMBINE_16(current_frame.payload[2], current_frame.payload[3]);
                break;

            case 0x0B1: // GR Misc Sensors
                can_rx_data.gr_misc_sensors.Brake =
                    (float)CAN_COMBINE_16(current_frame.payload[0], current_frame.payload[1])
                    * CAN_MSG_GR_MISC_SENSORS_SIG_BRAKE_FACTOR;
                break;

            default:
                break;
        }
    }

    driver->can_new_message_flag =
        (driver->rx_ring_buffer.head != driver->rx_ring_buffer.tail) ? 1U : 0U;
}
```

---

## Step 5: TX Payload Assembly (`can_comm.c`)

Populate buffer payloads prior to automated transmission:

```c
void set_can_frames(CAN_Driver_t* driver) {
    // Slot 0 (0x0E0): Inverter control signals
    driver->tx_message_frames[0].payload[0] = can_tx_data.ecu_inverter_sig.Motor_EN_DIS_Regen;
    driver->tx_message_frames[0].payload[1] = can_tx_data.ecu_inverter_sig.Inverter_L_Regen_Percentage;

    // Split multi-byte variables using byte extraction helpers
    uint16_t speed = pack_s16_scaled(velocity, 100.0f);
    driver->tx_message_frames[6].payload[0] = HIGH_BYTE(speed);
    driver->tx_message_frames[6].payload[1] = LOW_BYTE(speed);
}
```

---

## Step 6: Main Loop Integration (`main.c`)

Call driver functions inside application entry:

```c
// Peripheral setup & driver initialization
init_canbus_driver(&can_driver,
    (CanTxFn_t)HAL_FDCAN_AddMessageToTxFifoQ,
    (CanTxFifoLevelFn_t)HAL_FDCAN_GetTxFifoFreeLevel,
    (void*)&hfdcan1);

// Application execution loop
while (1) {
    process_can_frames(&can_driver);
    set_can_frames(&can_driver);

    CAN_send_frames(&can_driver, HAL_GetTick());
    if (can_driver.tx_queue_drain_requested) {
        CAN_process_tx_queue(&can_driver, 0U);
    }
}
```

---

## Common Implementation Issues

| Issue | Symptom | Solution |
|-------|---------|----------|
| Unset `rx_frame_number` / `tx_frame_number` | Ring buffer size evaluates to 0; messages dropped | Assign frame counts prior to `CAN_set_structures()` execution |
| ISR message processing | High interrupt latency; missed message frames | Limit ISR routine to `CAN_driver_rx_callback()`; process in main loop |
| Omitted `CAN_process_tx_queue()` call | Software buffer overflow; transmission halts | Ensure `CAN_process_tx_queue()` runs conditionally on `tx_queue_drain_requested` |
| Invalid DLC configuration | Truncated or malformed frames | Use hardware DLC definitions (`FDCAN_DLC_BYTES_8`) rather than raw byte counts |
| Manual edits to `can_signal_defs.h` | Code overwritten during build step | Update source DBC (`MAIN_DBC.dbc`) and regenerate headers |
