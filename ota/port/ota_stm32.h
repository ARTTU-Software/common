#ifndef INC_OTA_STM32_H
#define INC_OTA_STM32_H

/*
 * STM32 + FDCAN port of the OTA driver (G4, H5, L5).
 *
 * The family is detected from the HAL that is on the include path, so a
 * board only needs its normal CubeMX include directories. Everything that
 * differs per family is flash handling (port/ota_flash_stm32<fam>.c); CAN,
 * the app->bootloader handshake and the jump are shared.
 *
 * Application side:
 *     ota_app_stm32_init(&hfdcan1, OTA_BOARD_NODE_ID, my_is_safe_fn);
 *     ...and pass OTA_ID_ENTER_REQUEST / OTA_ID_CMD(node) frames to
 *     ota_app_handle_frame() from main-loop/task context.
 *
 * Bootloader side (after the CubeMX init, FDCAN initialised but not started):
 *     static const ota_layout_t layout = OTA_BOARD_LAYOUT;
 *     ota_bootloader_run(&hfdcan1, &layout);   // never returns
 */

#if defined(__has_include)
#  if __has_include("stm32h5xx_hal.h")
#    include "stm32h5xx_hal.h"
#    define OTA_STM32_H5 1
#  elif __has_include("stm32g4xx_hal.h")
#    include "stm32g4xx_hal.h"
#    define OTA_STM32_G4 1
#  elif __has_include("stm32l5xx_hal.h")
#    include "stm32l5xx_hal.h"
#    define OTA_STM32_L5 1
#  endif
#endif

#if defined(OTA_STM32_H5) || defined(OTA_STM32_G4) || defined(OTA_STM32_L5)
#define OTA_STM32_SUPPORTED 1

#include <stdbool.h>
#include <stdint.h>

#include "ota_boot.h"

/* How long the bootloader listens for a host after reset before starting a valid app (ms). */
#ifndef OTA_BOOT_LISTEN_MS
#define OTA_BOOT_LISTEN_MS  50U
#endif

/* ── application side ─────────────────────────────────────────── */

/**
 * Hook the OTA enter/ping handling to a running FDCAN instance.
 * @param is_safe_to_update  NULL = always allow. Return false to refuse (ERR_NOT_SAFE).
 */
void ota_app_stm32_init(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, bool (*is_safe_to_update)(void));

/* ── bootloader side ──────────────────────────────────────────── */

/**
 * Bootloader entry point. Requirements on the bootloader CubeMX project:
 *   - FDCAN initialised at the car bitrate, Std Filters Nbr >= 2, not started
 *   - no FDCAN interrupts used (it polls); ICACHE may stay off
 * erase_unit/program_unit and the app end are taken from the real part, the
 * board layout only provides addresses.
 */
void ota_bootloader_run(FDCAN_HandleTypeDef *hfdcan, const ota_layout_t *layout);

/** Weak hook, called on every bootloader loop iteration (e.g. blink an LED by state). */
void ota_bootloader_on_poll(uint8_t ota_state, uint32_t tick_ms);

/* ── family flash interface (port/ota_flash_stm32*.c) ─────────── */

bool     ota_stm32_flash_erase(uint32_t addr);   /* erase the erase unit that contains addr */
bool     ota_stm32_flash_program(uint32_t addr, const uint8_t *data, uint32_t len);
uint32_t ota_stm32_flash_end(void);              /* FLASH_BASE + real flash size */
uint32_t ota_stm32_flash_erase_unit(void);       /* sector / page size of this part */
uint32_t ota_stm32_flash_program_unit(void);     /* 16 (H5 quad-word) or 8 (G4/L5 double-word) */

#endif /* family supported */

#endif /* INC_OTA_STM32_H */
