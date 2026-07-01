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

## State and Flow Diagram

<div data-zoom="0.3">>

```mermaid
%%{init: {
  'theme': 'dark',
  'themeVariables': {
    'fontSize': '14px',
    'fontFamily': 'system-ui, sans-serif'
  },
  'flowchart': {
    'nodeSpacing': 30,
    'rankSpacing': 40,
    'padding': 15,
    'curve': 'basis'
  }
}}%%
flowchart TB
    %% Subgraph Styles
    classDef initStyle fill:#1e293b,stroke:#3b82f6,stroke-width:2px,color:#fff;
    classDef rxStyle fill:#2e1065,stroke:#a855f7,stroke-width:2px,color:#fff;
    classDef txStyle fill:#064e3b,stroke:#10b981,stroke-width:2px,color:#fff;
    classDef fifoStyle fill:#431407,stroke:#f97316,stroke-width:2px,color:#fff;
    classDef singleStyle fill:#451a03,stroke:#eab308,stroke-width:2px,color:#fff;
    classDef helperStyle fill:#1e293b,stroke:#64748b,stroke-width:2px,color:#fff;
    classDef dlcStyle fill:#083344,stroke:#06b6d4,stroke-width:2px,color:#fff;

    %% Node Styles
    classDef startNode fill:#1d4ed8,stroke:#3b82f6,stroke-width:1px,color:#fff;
    classDef decisionNode fill:#c2410c,stroke:#ea580c,stroke-width:1px,color:#fff;
    classDef errorNode fill:#991b1b,stroke:#ef4444,stroke-width:1px,color:#fff;
    classDef actionNode fill:#1e293b,stroke:#475569,stroke-width:1px,color:#fff;
    classDef successNode fill:#065f46,stroke:#10b981,stroke-width:1px,color:#fff;
    classDef rxActionNode fill:#581c87,stroke:#8b5cf6,stroke-width:1px,color:#fff;
    classDef txActionNode fill:#065f46,stroke:#10b981,stroke-width:1px,color:#fff;
    classDef fifoActionNode fill:#7c2d12,stroke:#f97316,stroke-width:1px,color:#fff;
    classDef singleActionNode fill:#78350f,stroke:#f59e0b,stroke-width:1px,color:#fff;
    classDef helperActionNode fill:#334155,stroke:#64748b,stroke-width:1px,color:#fff;

    subgraph Initialization ["Initialization Path"]
        direction TB
        init_start(["CAN_set_structures called"]):::startNode
        init_check{"Driver or buffers null?"}:::decisionNode
        init_ret(["Return early"]):::errorNode
        init_store["Store buffer pointers"]:::actionNode
        init_indices["Initialize head and tail indices"]:::actionNode
        init_clear["Clear new message flag"]:::actionNode
        init_done(["Initialization complete"]):::successNode

        init_start --> init_check
        init_check -- Yes --> init_ret
        init_check -- No --> init_store
        init_store --> init_indices
        init_indices --> init_clear
        init_clear --> init_done
    end

    subgraph RX_Interrupt ["RX Interrupt Path"]
        direction TB
        rx_start(["RX callback triggered"]):::startNode
        rx_check{"Driver null?"}:::decisionNode
        rx_ret(["RX return early"]):::errorNode
        rx_len{"Payload length valid?"}:::decisionNode
        rx_drop_len(["Drop invalid payload"]):::errorNode
        rx_full{"RX buffer full?"}:::decisionNode
        rx_drop_full(["Drop frame overflow"]):::errorNode
        rx_dlc["Convert DLC to payload size"]:::rxActionNode
        rx_copy["Copy frame to RX ring buffer"]:::rxActionNode
        rx_inc["Increment RX head index"]:::rxActionNode
        rx_flag["Set new message flag"]:::rxActionNode
        rx_done(["RX complete"]):::successNode

        rx_start --> rx_check
        rx_check -- Yes --> rx_ret
        rx_check -- No --> rx_len
        rx_len -- No --> rx_drop_len
        rx_len -- Yes --> rx_full
        rx_full -- Yes --> rx_drop_full
        rx_full -- No --> rx_dlc
        rx_dlc --> rx_copy
        rx_copy --> rx_inc
        rx_inc --> rx_flag
        rx_flag --> rx_done
    end

    subgraph Periodic_TX ["Periodic TX Scheduler"]
        direction TB
        sched_start(["CAN_send_frames called"]):::startNode
        sched_check{"TX driver null?"}:::decisionNode
        sched_ret(["TX return early"]):::errorNode
        sched_loop["Iterate frame configs"]:::txActionNode
        sched_non_periodic{"Frame non periodic?"}:::decisionNode
        sched_skip["Skip frame"]:::helperActionNode
        sched_due{"Frame due this tick?"}:::decisionNode
        sched_enqueue["Enqueue TX frame"]:::txActionNode
        sched_update["Update scheduler timer"]:::txActionNode
        sched_more{"More frames?"}:::decisionNode
        sched_flush["Flush TX ring buffer"]:::txActionNode
        sched_done(["Periodic TX complete"]):::successNode

        sched_start --> sched_check
        sched_check -- Yes --> sched_ret
        sched_check -- No --> sched_loop
        sched_loop --> sched_non_periodic
        sched_non_periodic -- Yes --> sched_skip
        sched_non_periodic -- No --> sched_due
        sched_due -- No --> sched_skip
        sched_due -- Yes --> sched_enqueue
        sched_enqueue --> sched_update
        sched_update --> sched_more
        sched_skip --> sched_more
        sched_more -- Yes --> sched_loop
        sched_more -- No --> sched_flush
        sched_flush --> sched_done
    end

    subgraph TX_FIFO_Empty ["TX FIFO Empty Path"]
        direction TB
        fifo_start(["TX FIFO empty callback"]):::startNode
        fifo_check{"FIFO driver null?"}:::decisionNode
        fifo_ret(["FIFO return early"]):::errorNode
        fifo_flush["Call flush TX ring buffer"]:::fifoActionNode
        fifo_done(["FIFO callback complete"]):::successNode

        fifo_start --> fifo_check
        fifo_check -- Yes --> fifo_ret
        fifo_check -- No --> fifo_flush
        fifo_flush --> fifo_done
    end

    subgraph Single_Frame ["Single Frame Path"]
        direction TB
        single_start(["CAN_send_single_frame called"]):::startNode
        single_check{"Single driver null?"}:::decisionNode
        single_ret(["Single return early"]):::errorNode
        single_enqueue["Enqueue single frame"]:::singleActionNode
        single_flush["Flush after enqueue"]:::singleActionNode
        single_done(["Single frame complete"]):::successNode

        single_start --> single_check
        single_check -- Yes --> single_ret
        single_check -- No --> single_enqueue
        single_enqueue --> single_flush
        single_flush --> single_done
    end

    subgraph Helpers ["Shared Internal Helpers"]
        direction TB
        subgraph Enqueue_Helper ["Enqueue TX Frame Helper"]
            direction TB
            enq_start(["Enqueue TX frame helper"]):::startNode
            enq_check{"TX buffer full?"}:::decisionNode
            enq_drop(["Drop frame buffer full"]):::errorNode
            enq_dlc["Convert payload size to DLC"]:::helperActionNode
            enq_copy["Copy frame to TX ring buffer"]:::helperActionNode
            enq_inc["Increment TX head index"]:::helperActionNode
            enq_done(["Enqueue complete"]):::successNode

            enq_start --> enq_check
            enq_check -- Yes --> enq_drop
            enq_check -- No --> enq_dlc
            enq_dlc --> enq_copy
            enq_copy --> enq_inc
            enq_inc --> enq_done
        end

        subgraph Flush_Helper ["Flush TX Ring Buffer Helper"]
            direction TB
            flush_start(["Flush TX ring buffer helper"]):::startNode
            flush_check{"TX buffer empty?"}:::decisionNode
            flush_none(["Nothing to flush"]):::helperActionNode
            flush_fifo["Check FIFO free level"]:::helperActionNode
            flush_fifo_space{"FIFO has space?"}:::decisionNode
            flush_wait(["Wait for FIFO"]):::helperActionNode
            flush_cb["Call add to FIFO callback"]:::helperActionNode
            flush_success{"Add to FIFO success?"}:::decisionNode
            flush_inc["Increment TX tail index"]:::helperActionNode
            flush_fail(["Add failed retry later"]):::errorNode
            flush_more{"More frames to flush?"}:::decisionNode
            flush_done(["Flush complete"]):::successNode

            flush_start --> flush_check
            flush_check -- Yes --> flush_none
            flush_check -- No --> flush_fifo
            flush_fifo --> flush_fifo_space
            flush_fifo_space -- No --> flush_wait
            flush_fifo_space -- Yes --> flush_cb
            flush_cb --> flush_success
            flush_success -- Yes --> flush_inc
            flush_inc --> flush_more
            flush_more -- Yes --> flush_fifo
            flush_more -- No --> flush_done
        end
    end

    subgraph DLC_Helpers ["DLC Conversion Helpers"]
        direction TB
        subgraph Size_To_DLC ["Payload size to DLC"]
            direction TB
            size_to_dlc_start(["Payload size to DLC"]):::startNode --> map_to_dlc["Map size to DLC value"]:::helperActionNode
            ret_dlc(["Return DLC"]):::successNode
            map_to_dlc --> ret_dlc
        end
        subgraph DLC_To_Size ["DLC to payload size"]
            direction TB
            dlc_to_size_start(["DLC to payload size"]):::startNode --> map_to_size["Map DLC to size value"]:::helperActionNode
            ret_size(["Return payload size"]):::successNode
            map_to_size --> ret_size
        end
    end

    %% Cross-group connections (simplified/styled to prevent layout chaos)
    sched_enqueue -.-> enq_start
    single_enqueue -.-> enq_start
    
    sched_flush -.-> flush_start
    fifo_flush -.-> flush_start
    single_flush -.-> flush_start

    rx_dlc -.-> dlc_to_size_start
    enq_dlc -.-> size_to_dlc_start

    %% Subgraph Styling Assignments
    style Initialization fill:#0f172a,stroke:#3b82f6,stroke-width:2px,color:#fff
    style RX_Interrupt fill:#0f172a,stroke:#a855f7,stroke-width:2px,color:#fff
    style Periodic_TX fill:#0f172a,stroke:#10b981,stroke-width:2px,color:#fff
    style TX_FIFO_Empty fill:#0f172a,stroke:#f97316,stroke-width:2px,color:#fff
    style Single_Frame fill:#0f172a,stroke:#eab308,stroke-width:2px,color:#fff
    style Helpers fill:#0f172a,stroke:#64748b,stroke-width:2px,color:#fff
    style Enqueue_Helper fill:#1e293b,stroke:#475569,stroke-width:1px,color:#fff
    style Flush_Helper fill:#1e293b,stroke:#475569,stroke-width:1px,color:#fff
    style DLC_Helpers fill:#0f172a,stroke:#06b6d4,stroke-width:2px,color:#fff
    style Size_To_DLC fill:#1e293b,stroke:#475569,stroke-width:1px,color:#fff
    style DLC_To_Size fill:#1e293b,stroke:#475569,stroke-width:1px,color:#fff
```

</div>

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