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

> [!NOTE]
> This driver uses HAL function callbacks to keep the driver truly generic. 

## Structure
> [!NOTE]
> You should have a `can_driver.c` and `can_comm.c` pair of files to manage the driver functions/initialization and the actual CAN communication handling.

## State Diagram
![CAN Driver Diagram](../../../../../public/images/CAN_driver_diagram.png)

## Functions
#### `void CAN_set_structures`
Initializes the driver with the hardware handle and callback functions.

#### `void CAN_driver_rx_callback`
ISR-safe RX callback that writes a received frame into the RX ring buffer.

#### `void CAN_send_frames`
Schedules periodic frames based on per-frame scheduler timers.
```c
static void process_can_helper(void){
    process_can_frames(&can_driver);
    CAN_send_frames(&can_driver, HAL_GetTick());
    if (can_driver.tx_queue_drain_requested) {
      CAN_process_tx_queue(&can_driver, 0);
    }
}
```

#### `uint16_t CAN_process_tx_queue`
Drains queued TX frames into the hardware FIFO while space is available.

#### `uint32_t CAN_send_single_frame`
Queues a single frame for transmission. Returns 0 on success, 1 on failure.
```c
if(CAN_send_single_frame(&can_driver, &status_frame) != 0){
    CAN_process_tx_queue(&can_driver, 5);
    CAN_send_single_frame(&can_driver, &status_frame);
}
```

## can_driver.c usage example
For an example can_driver.c file:
```c
#include "can_driver.h"
#include "generic_can_driver.h"

#define HZ_TO_MS(hz) (1000 / (hz))

FDCAN_TxHeaderTypeDef tx_header_templates[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_message_frames[NR_OF_CAN_TX_FRAMES];
CAN_Tx_Message_Frame_t tx_queue_frames[NR_OF_CAN_TX_BUFFER_FRAMES];
CAN_Rx_Message_Frame_t rx_message_frame[NR_OF_CAN_RX_BUFFER_FRAMES];

CAN_Driver_t can_driver;

void init_canbus_driver(CAN_Driver_t* driver, CanTxFn_t add_to_fifo_fn, CanTxFifoLevelFn_t fifo_level_fn, void* hfdcan_instance){
    driver->rx_frame_number = NR_OF_CAN_RX_BUFFER_FRAMES;
    driver->tx_frame_number = NR_OF_CAN_TX_FRAMES;
    driver->tx_message_frames = tx_message_frames;
    driver->tx_ring_buffer.frame = tx_queue_frames;
    driver->tx_ring_buffer.size = NR_OF_CAN_TX_BUFFER_FRAMES;
    driver->rx_ring_buffer.frame = rx_message_frame;

    set_tx_configs(driver);

    for(int i = 0; i < NR_OF_CAN_TX_FRAMES; i++){
        tx_header_templates[i].IdType = FDCAN_STANDARD_ID;
        tx_header_templates[i].TxFrameType = FDCAN_DATA_FRAME;
        tx_header_templates[i].ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        tx_header_templates[i].BitRateSwitch = FDCAN_BRS_OFF;
        tx_header_templates[i].FDFormat = FDCAN_CLASSIC_CAN;
        tx_header_templates[i].TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        tx_header_templates[i].MessageMarker = 0;
        tx_header_templates[i].DataLength = driver->tx_message_frames[i].num_values;
        tx_header_templates[i].Identifier = driver->tx_message_frames[i].msg_id;

        driver->tx_message_frames[i].hdr = &tx_header_templates[i];
    }

    CAN_set_structures(driver, add_to_fifo_fn, fifo_level_fn, hfdcan_instance);
}

static void set_tx_configs(CAN_Driver_t* driver){
    // This sets the frame configurations
    driver->tx_message_frames[0].msg_id = 0x0A0;
    driver->tx_message_frames[0].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[0].previous_tick = 0;
    driver->tx_message_frames[0].scheduler_timer_value = HZ_TO_MS(100);

    driver->tx_message_frames[1].msg_id = 0x0A1;
    driver->tx_message_frames[1].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[1].previous_tick = 0;
    driver->tx_message_frames[1].scheduler_timer_value = HZ_TO_MS(100);

    driver->tx_message_frames[2].msg_id = 0x0AF;
    driver->tx_message_frames[2].num_values = FDCAN_DLC_BYTES_8;
    driver->tx_message_frames[2].previous_tick = 0;
    // Non-periodic = scheduler skips the frame entirely, is sent via CAN_send_single_frame
    driver->tx_message_frames[2].scheduler_timer_value = CAN_DRIVER_NON_PERIODIC_FRAME;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs){
    while(HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0){
        if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0){
            FDCAN_RxHeaderTypeDef can_rx_hdr;
            uint8_t data[8];
            HAL_StatusTypeDef status = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &can_rx_hdr, data);
            if (status != HAL_OK){
                // do nothing
            }
            // Pass received message to CAN driver callback for processing and storage in ring buffer
            CAN_driver_rx_callback(&can_driver, 
                data, 
                (void*)&can_rx_hdr, 
                can_rx_hdr.Identifier, 
                can_rx_hdr.DataLength,
                HAL_GetTick());
        }
    }
}

// For TX FIFO callback functionality (draining part)
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == &hfdcan2) {
        can_driver.tx_queue_drain_requested = 1;
    }
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    (void)BufferIndexes;

    if (hfdcan == &hfdcan2) {
        can_driver.tx_queue_drain_requested = 1;
    }
}
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  if((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0)  /* If Bus-Off error occured */
  {
     hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT; /* Recover from Bus-Off */
  }
}
```

> [!WARNING]
> `CAN_driver_rx_callback` is intended for ISR context; do not do heavy work inside it.

## can_comm.c usage example
For an example can_comm.c file:
```c
#include "can_comm.h"
#include "can_driver.h"

void process_can_frames(CAN_Driver_t* driver){
    while (driver->rx_ring_buffer.head != driver->rx_ring_buffer.tail) {
        // Snapshot frame so that it is not overwritten at some point
        CAN_Rx_Message_Frame_t current_frame = driver->rx_ring_buffer.frame[driver->rx_ring_buffer.tail];
        driver->rx_ring_buffer.tail = (driver->rx_ring_buffer.tail + 1) % driver->rx_ring_buffer.size;

        switch(current_frame.msg_id){
            case 0x0A2:
                // do something
                can_input_data.front_nmos.R2D_LED_Delay = current_frame.payload[0];
                break;
            case 0x0A3:
                // do something else
                break;
            default:
                // Unknown ID, ignore
                break;
        }
    }

    driver->can_new_message_flag = 0;
}

void set_can_frames(CAN_Driver_t* driver){
    // Wheel Speed FR
    driver->tx_message_frames[0].payload[0] = HIGH_BYTE((uint16_t)mx5051_1.rpm);
    driver->tx_message_frames[0].payload[1] = LOW_BYTE((uint16_t)mx5051_1.rpm);

    // // Wheel Speed FL
    driver->tx_message_frames[0].payload[2] = HIGH_BYTE((uint16_t)mx5051_2.rpm);
    driver->tx_message_frames[0].payload[3] = LOW_BYTE((uint16_t)mx5051_2.rpm);

    // // Suspension Travel FR
    driver->tx_message_frames[0].payload[4] = HIGH_BYTE((uint16_t)kpm12j1.displacement * 1000);
    driver->tx_message_frames[0].payload[5] = LOW_BYTE((uint16_t)kpm12j1.displacement * 1000);

    // // Suspension Travel FL
    driver->tx_message_frames[0].payload[6] = HIGH_BYTE((uint16_t)kpm12j2.displacement * 1000);
    driver->tx_message_frames[0].payload[7] = LOW_BYTE((uint16_t)kpm12j2.displacement * 1000);
}
```