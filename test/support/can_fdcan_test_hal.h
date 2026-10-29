#ifndef INC_CAN_FDCAN_TEST_HAL_H
#define INC_CAN_FDCAN_TEST_HAL_H

#include <stdint.h>

/* Deliberately use the older shifted representation to catch raw-length casts. */
#define CAN_DRIVER_STM32_FDCAN_HEADER "can_fdcan_test_hal.h"
#define FDCAN_DLC_BYTES_0 (0U << 16U)
#define FDCAN_DLC_BYTES_1 (1U << 16U)
#define FDCAN_DLC_BYTES_2 (2U << 16U)
#define FDCAN_DLC_BYTES_3 (3U << 16U)
#define FDCAN_DLC_BYTES_4 (4U << 16U)
#define FDCAN_DLC_BYTES_5 (5U << 16U)
#define FDCAN_DLC_BYTES_6 (6U << 16U)
#define FDCAN_DLC_BYTES_7 (7U << 16U)
#define FDCAN_DLC_BYTES_8 (8U << 16U)
#define FDCAN_STANDARD_ID 0U
#define FDCAN_EXTENDED_ID 1U
#define FDCAN_DATA_FRAME 0U
#define FDCAN_REMOTE_FRAME 1U
#define FDCAN_CLASSIC_CAN 0U
#define FDCAN_FD_CAN 1U
#define FDCAN_ESI_ACTIVE 0U
#define FDCAN_BRS_OFF 0U
#define FDCAN_NO_TX_EVENTS 0U
#define FDCAN_RX_FIFO0 0U
#define FDCAN_RX_FIFO1 1U

typedef struct { uint32_t instance; } FDCAN_HandleTypeDef;
typedef enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT } HAL_StatusTypeDef;
typedef struct {
    uint32_t Identifier, IdType, TxFrameType, DataLength, ErrorStateIndicator;
    uint32_t BitRateSwitch, FDFormat, TxEventFifoControl, MessageMarker;
} FDCAN_TxHeaderTypeDef;
typedef struct {
    uint32_t Identifier, IdType, RxFrameType, DataLength, FDFormat;
} FDCAN_RxHeaderTypeDef;

uint32_t HAL_FDCAN_GetTxFifoFreeLevel(FDCAN_HandleTypeDef *handle);
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *handle,
    const FDCAN_TxHeaderTypeDef *header, const uint8_t *data);
uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *handle, uint32_t fifo);
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *handle,
    uint32_t fifo, FDCAN_RxHeaderTypeDef *header, uint8_t *data);

#endif /* INC_CAN_FDCAN_TEST_HAL_H */
