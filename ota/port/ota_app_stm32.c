#include "ota_stm32_common.h"

#if defined(OTA_STM32_SUPPORTED)

#include "ota_app.h"

static FDCAN_HandleTypeDef *ota_app_hfdcan;
static ota_app_config_t ota_app_cfg;

/*
 * OTA replies bypass the board's CAN queue on purpose: they are rare and the
 * enter-bootloader reply must be on the wire before the reset. Call
 * ota_app_handle_frame() from the same context as the board's other TX code
 * (main loop / CAN task), not from an ISR.
 */
static bool ota_app_stm32_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    return ota_stm32_can_send(ota_app_hfdcan, id, data, len);
}

static void ota_app_stm32_enter_bootloader(void)
{
    ota_stm32_can_drain_tx(ota_app_hfdcan); /* let the ENTER reply leave before the reset */
    ota_stm32_boot_flag_set();              /* read and cleared by the bootloader */
    NVIC_SystemReset();
}

void ota_app_stm32_init(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, bool (*is_safe_to_update)(void))
{
    ota_app_hfdcan = hfdcan;
    ota_app_cfg.node_id = node_id;
    ota_app_cfg.send = ota_app_stm32_send;
    ota_app_cfg.is_safe_to_update = is_safe_to_update;
    ota_app_cfg.enter_bootloader = ota_app_stm32_enter_bootloader;
    ota_app_init(&ota_app_cfg);
}

#endif /* OTA_STM32_SUPPORTED */
