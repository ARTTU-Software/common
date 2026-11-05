#ifndef INC_OTA_STM32_COMMON_H
#define INC_OTA_STM32_COMMON_H

/* Internal to the STM32 port: FDCAN and backup-register helpers shared by app and bootloader. */

#include "ota_stm32.h"

#if defined(OTA_STM32_SUPPORTED)

#define OTA_STM32_TX_FIFO_SIZE   3U    /* FDCAN TX FIFO elements, same on G4/H5/L5 */
#define OTA_STM32_TX_WAIT_MS     5U    /* max wait for a free TX slot */
#define OTA_STM32_TX_DRAIN_MS    20U   /* max wait for the TX FIFO to empty before a reset */

bool ota_stm32_can_send(FDCAN_HandleTypeDef *hfdcan, uint32_t id, const uint8_t *data, uint8_t len);
bool ota_stm32_can_recv(FDCAN_HandleTypeDef *hfdcan, uint32_t *id, uint8_t *data, uint8_t *len);
void ota_stm32_can_drain_tx(FDCAN_HandleTypeDef *hfdcan);

/* App -> bootloader request flag in TAMP->BKP0R (survives a system reset). */
void ota_stm32_boot_flag_set(void);
bool ota_stm32_boot_flag_take(void);   /* read and clear */

#endif

#endif /* INC_OTA_STM32_COMMON_H */
