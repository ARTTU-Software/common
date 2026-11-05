#include "ota_stm32_common.h"

#if defined(OTA_STM32_SUPPORTED)

#include "ota_protocol.h"

/* DataLength is an encoded field whose encoding changed between HAL versions: go through the macros. */
static const uint32_t ota_dlc_codes[9] = {
    FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
    FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
    FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8,
};

static uint8_t ota_len_from_dlc(uint32_t dlc)
{
    for (uint8_t len = 0U; len < 9U; len++) {
        if (ota_dlc_codes[len] == dlc) {
            return len;
        }
    }
    return 8U; /* CAN FD lengths are clipped: OTA frames are classic, 8 bytes */
}

bool ota_stm32_can_send(FDCAN_HandleTypeDef *hfdcan, uint32_t id, const uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef hdr = {
        .Identifier          = id,
        .IdType              = FDCAN_STANDARD_ID,
        .TxFrameType         = FDCAN_DATA_FRAME,
        .DataLength          = ota_dlc_codes[(len <= 8U) ? len : 8U],
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch       = FDCAN_BRS_OFF,
        .FDFormat            = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl  = FDCAN_NO_TX_EVENTS,
        .MessageMarker       = 0U,
    };
    uint32_t t0 = HAL_GetTick();

    while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) == 0U) {
        if ((HAL_GetTick() - t0) >= OTA_STM32_TX_WAIT_MS) {
            return false;
        }
    }
    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &hdr, (uint8_t *)data) == HAL_OK;
}

bool ota_stm32_can_recv(FDCAN_HandleTypeDef *hfdcan, uint32_t *id, uint8_t *data, uint8_t *len)
{
    FDCAN_RxHeaderTypeDef hdr;

    if (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) == 0U) {
        return false;
    }
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &hdr, data) != HAL_OK) {
        return false;
    }
    *id = hdr.Identifier;
    *len = ota_len_from_dlc(hdr.DataLength);
    return true;
}

void ota_stm32_can_drain_tx(FDCAN_HandleTypeDef *hfdcan)
{
    uint32_t t0 = HAL_GetTick();

    while ((HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) < OTA_STM32_TX_FIFO_SIZE) &&
           ((HAL_GetTick() - t0) < OTA_STM32_TX_DRAIN_MS)) {
    }
}

static void ota_stm32_backup_access(void)
{
#if defined(__HAL_RCC_PWR_CLK_ENABLE)
    __HAL_RCC_PWR_CLK_ENABLE();   /* G4/L5: PWR is on a gated APB clock */
#endif
    __HAL_RCC_RTCAPB_CLK_ENABLE(); /* TAMP registers */
    HAL_PWR_EnableBkUpAccess();
}

void ota_stm32_boot_flag_set(void)
{
    ota_stm32_backup_access();
    TAMP->BKP0R = OTA_BOOT_FLAG_MAGIC;
}

bool ota_stm32_boot_flag_take(void)
{
    bool requested;

    ota_stm32_backup_access();
    requested = (TAMP->BKP0R == OTA_BOOT_FLAG_MAGIC);
    TAMP->BKP0R = 0U; /* one-shot: a later plain reset boots the app again */
    return requested;
}

#endif /* OTA_STM32_SUPPORTED */
