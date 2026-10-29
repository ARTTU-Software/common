#include "can_stm32_fdcan.h"

/* Opt in per build, so a host or another MCU can compile common without HAL. */
#if defined(CAN_DRIVER_STM32_FDCAN_HEADER)
#include CAN_DRIVER_STM32_FDCAN_HEADER

#include <stddef.h>
#include <string.h>

/* HAL releases use both shifted and unshifted DLC encodings. Use their constants. */
static const uint32_t can_stm32_fdcan_dlc[] = {
    FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
    FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
    FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8
};

can_driver_status_t can_stm32_fdcan_submit(void *context,
    const can_driver_frame_t *frame)
{
    if (context == NULL || frame == NULL || frame->length > CAN_DRIVER_PAYLOAD_BYTES
        || frame->id > (frame->extended ? 0x1fffffffU : 0x7ffU)) {
        return CAN_DRIVER_INVALID;
    }
    FDCAN_HandleTypeDef *handle = context;
    if (HAL_FDCAN_GetTxFifoFreeLevel(handle) == 0U) {
        return CAN_DRIVER_BUSY;
    }

    FDCAN_TxHeaderTypeDef header = {0};
    header.Identifier = frame->id;
    header.IdType = frame->extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    header.TxFrameType = FDCAN_DATA_FRAME;
    header.DataLength = can_stm32_fdcan_dlc[frame->length];
    header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    header.BitRateSwitch = FDCAN_BRS_OFF;
    header.FDFormat = FDCAN_CLASSIC_CAN;
    header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    /* HAL versions disagree on payload constness. Give all of them a local copy. */
    uint8_t payload[CAN_DRIVER_PAYLOAD_BYTES];
    memcpy(payload, frame->data, sizeof(payload));
    const HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(handle, &header, payload);
    if (status == HAL_BUSY) {
        return CAN_DRIVER_BUSY;
    }
    return status == HAL_OK ? CAN_DRIVER_OK : CAN_DRIVER_IO_ERROR;
}

can_driver_status_t can_stm32_fdcan_receive_isr(can_driver_t *driver,
    void *context, uint32_t fifo, uint32_t now_ms, uint16_t budget)
{
    if (driver == NULL || context == NULL || budget == 0U
        || (fifo != FDCAN_RX_FIFO0 && fifo != FDCAN_RX_FIFO1)) {
        return CAN_DRIVER_INVALID;
    }
    if (!driver->initialized) {
        return CAN_DRIVER_NOT_INITIALIZED;
    }
    FDCAN_HandleTypeDef *handle = context;
    can_driver_status_t result = CAN_DRIVER_OK;
    for (uint16_t i = 0U; i < budget; i++) {
        if (HAL_FDCAN_GetRxFifoFillLevel(handle, fifo) == 0U) {
            break;
        }
        FDCAN_RxHeaderTypeDef header = {0};
        uint8_t payload[64];
        if (HAL_FDCAN_GetRxMessage(handle, fifo, &header, payload) != HAL_OK) {
            return CAN_DRIVER_IO_ERROR;
        }
        if (header.RxFrameType != FDCAN_DATA_FRAME || header.FDFormat != FDCAN_CLASSIC_CAN) {
            result = CAN_DRIVER_INVALID;
            continue;
        }

        uint8_t length = 0U;
        while (length <= CAN_DRIVER_PAYLOAD_BYTES
            && can_stm32_fdcan_dlc[length] != header.DataLength) {
            length++;
        }
        if (length > CAN_DRIVER_PAYLOAD_BYTES) {
            result = CAN_DRIVER_INVALID;
            continue;
        }
        can_driver_frame_t frame = {0};
        frame.id = header.Identifier;
        frame.extended = header.IdType == FDCAN_EXTENDED_ID;
        frame.length = length;
        memcpy(frame.data, payload, length);
        const can_driver_status_t status = can_driver_receive_isr(driver, &frame, now_ms);
        if (status != CAN_DRIVER_OK) {
            result = status;
        }
    }
    return result;
}

#endif /* CAN_DRIVER_STM32_FDCAN_HEADER */
