---
title: CAN Integration Guide
description: Step-by-step guide to integrating the generic CAN driver on a new board.
---

# CAN Integration Guide

This walks through how to set up the generic CAN driver on a new board. All examples are taken from the ECU project - the same pattern applies to every board.

## File structure

Every board using CAN needs these files:

```
Core/Inc/App/Communication/
├── can_driver.h    ← buffer sizes, externs, HAL callback prototypes
└── can_comm.h      ← RX/TX data structs, process/set function prototypes

Core/Src/App/Communication/
├── can_driver.c    ← buffer allocation, frame config, HAL callbacks, init
└── can_comm.c      ← RX message handler, TX payload packer
```

---

## Step 1 - Define buffer sizes (`can_driver.h`)

Decide how many TX frame slots, and how large the RX and TX ring buffers should be.

```c
#include "generic_can_driver.h"

#define HZ_TO_MS(hz) (1000 / (hz))

#define NR_OF_CAN_TX_FRAMES        10   // Number of distinct TX messages this board sends
#define NR_OF_CAN_RX_BUFFER_FRAMES 128  // RX ring buffer depth (handles ISR bursts)
#define NR_OF_CAN_TX_BUFFER_FRAMES 64   // TX queue depth (software → hardware FIFO)

extern CAN_Driver_t can_driver;

void init_canbus_driver(CAN_Driver_t *driver,
    CanTxFn_t add_to_fifo_fn,
    CanTxFifoLevelFn_t get_tx_fifo_level_fn,
    void *hfdcan_instance);
```

> [!TIP]
> **Sizing the RX buffer**: it needs to hold all frames that can arrive between two `process_can_frames()` calls. 128 is a safe default for a busy bus. If you drop frames, increase this.

---

## Step 2 - Allocate buffers and configure TX frames (`can_driver.c`)

This is where you allocate the static arrays and tell each TX frame slot its CAN ID, DLC, and transmission rate.

```c
#include "can_driver.h"
#include "generic_can_driver.h"

// Static buffer allocations
FDCAN_TxHeaderTypeDef tx_header_templates[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_message_frames[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_buffer_frames[NR_OF_CAN_TX_BUFFER_FRAMES];
CAN_Rx_Message_Frame_t rx_message_frame[NR_OF_CAN_RX_BUFFER_FRAMES];

CAN_Driver_t can_driver;
```

### Configuring TX frame slots

Each slot maps to one CAN message your board sends. Set the CAN ID, payload size, and scheduler period:

```c
static void set_tx_configs(CAN_Driver_t* driver) {
    // Periodic frame: sent automatically at 50 Hz by the scheduler
    driver->tx_message_frames[0].msg_id = 0x0E0;
    driver->tx_message_frames[0].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[0].scheduler_timer_value = HZ_TO_MS(50); // 20ms

    // Non-periodic frame: only sent manually via CAN_send_single_frame()
    driver->tx_message_frames[1].msg_id = 0x0B3;
    driver->tx_message_frames[1].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[1].scheduler_timer_value = CAN_DRIVER_NON_PERIODIC_FRAME;

    // ... repeat for each TX message
}
```

**Periodic vs. non-periodic**: Use `HZ_TO_MS(freq)` for frames that should be sent at a fixed rate (sensor data, status). Use `CAN_DRIVER_NON_PERIODIC_FRAME` for event-driven frames (commands, one-shot status updates) - these are sent via `CAN_send_single_frame()`.

### Init function

Wire the buffers to the driver struct, fill in the HAL TX header templates, then call the generic init:

```c
void init_canbus_driver(CAN_Driver_t* driver,
    CanTxFn_t add_to_fifo_fn,
    CanTxFifoLevelFn_t get_tx_fifo_level_fn,
    void* hfdcan_instance)
{
    // Tell the driver how many frames and buffers it has
    driver->rx_frame_number = NR_OF_CAN_RX_BUFFER_FRAMES;
    driver->tx_frame_number = NR_OF_CAN_TX_FRAMES;

    // Point to the static arrays
    driver->tx_message_frames = tx_message_frames;
    driver->tx_ring_buffer.frame = tx_buffer_frames;
    driver->tx_ring_buffer.size = NR_OF_CAN_TX_BUFFER_FRAMES;
    driver->rx_ring_buffer.frame = rx_message_frame;

    set_tx_configs(driver);

    // Build HAL TX headers from the frame configs
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

        // Generic driver stores the header as void* - cast happens here
        driver->tx_message_frames[i].hdr = (void*)&tx_header_templates[i];
    }

    // Initialize the generic driver internals (resets indices, clears flags)
    CAN_set_structures(driver, add_to_fifo_fn, get_tx_fifo_level_fn, hfdcan_instance);
}
```

---

## Step 3 - HAL callbacks (`can_driver.c`)

These callbacks bridge the STM32 HAL interrupts to the generic driver. They live in `can_driver.c` alongside the init.

### RX callback

Drains the hardware RX FIFO and feeds each frame to the generic driver's ring buffer:

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

### TX drain callbacks

When the hardware FIFO has space again, signal the driver to drain its software queue:

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

### Bus-Off recovery

```c
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs) {
    if ((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0) {
        hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT; // Auto-recover from Bus-Off
    }
}
```

---

## Step 4 - RX message handler (`can_comm.c`)

Define structs to hold the latest RX and TX data, then write `process_can_frames()` with a `switch` on the message ID:

```c
// can_comm.h
typedef struct {
    can_msg_GF_Wheel_Sensors_t gf_wheel_sensors;
    can_msg_GR_Misc_Sensors_t  gr_misc_sensors;
    // ... one field per RX message type you care about
} CAN_data_rx_t;

extern volatile CAN_data_rx_t can_rx_data;
```

```c
// can_comm.c
volatile CAN_data_rx_t can_rx_data;

void process_can_frames(CAN_Driver_t* driver) {
    while (driver->rx_ring_buffer.head != driver->rx_ring_buffer.tail) {
        // Snapshot the frame so the ISR can't overwrite it mid-read
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
                break; // Unknown ID - ignore
        }
    }

    driver->can_new_message_flag =
        (driver->rx_ring_buffer.head != driver->rx_ring_buffer.tail) ? 1U : 0U;
}
```

> [!NOTE]
> Use the `CAN_COMBINE_16` / `CAN_COMBINE_32` macros from `generic_can_driver.h` and the `_FACTOR` macros from `can_signal_defs.h` to reconstruct and scale signal values.

---

## Step 5 - TX payload packer (`can_comm.c`)

Pack your outgoing data into the TX frame payloads before the scheduler sends them:

```c
void set_can_frames(CAN_Driver_t* driver) {
    // Frame 0 (0x0E0): Inverter command
    driver->tx_message_frames[0].payload[0] = can_tx_data.ecu_inverter_sig.Motor_EN_DIS_Regen;
    driver->tx_message_frames[0].payload[1] = can_tx_data.ecu_inverter_sig.Inverter_L_Regen_Percentage;
    // ... pack remaining bytes

    // For 16-bit values, split into HIGH_BYTE / LOW_BYTE
    uint16_t speed = pack_s16_scaled(velocity, 100.0f);
    driver->tx_message_frames[6].payload[0] = HIGH_BYTE(speed);
    driver->tx_message_frames[6].payload[1] = LOW_BYTE(speed);
}
```

---

## Step 6 - Wire into `main.c`

```c
// In main(), after peripheral init:
init_canbus_driver(&can_driver,
    (CanTxFn_t)HAL_FDCAN_AddMessageToTxFifoQ,
    (CanTxFifoLevelFn_t)HAL_FDCAN_GetTxFifoFreeLevel,
    (void*)&hfdcan1);

// Main loop:
while (1) {
    process_can_frames(&can_driver);   // Consume RX ring buffer
    set_can_frames(&can_driver);       // Pack TX payloads with latest data

    // Service TX scheduler + drain queue
    CAN_send_frames(&can_driver, HAL_GetTick());
    if (can_driver.tx_queue_drain_requested) {
        CAN_process_tx_queue(&can_driver, 0U);
    }

    // ... rest of your loop (FSM, sensors, etc.)
}
```

---

## Common Pitfalls

| Pitfall | What happens | Fix |
|---------|-------------|-----|
| Forgetting to set `rx_frame_number` / `tx_frame_number` before `CAN_set_structures` | RX buffer size is 0, all frames dropped | Set them in `init_canbus_driver` before calling the generic init |
| Processing frames inside the RX callback | Blocks interrupts, causes frame loss | Only call `CAN_driver_rx_callback` in the ISR - do all processing in the main loop |
| Not calling `CAN_process_tx_queue` | Frames pile up in the software queue and never reach the bus | Call it every loop iteration, gated by `tx_queue_drain_requested` |
| Using wrong DLC constant | Payload bytes are truncated or padded | Use `FDCAN_DLC_BYTES_8` (not raw `8`) for the `num_values` field |
| Editing `can_signal_defs.h` by hand | Changes are overwritten on next DBC generation | Edit `MAIN_DBC.dbc` and re-run `dbc_to_c_builder.py` |
