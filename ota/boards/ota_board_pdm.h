#ifndef INC_OTA_BOARD_PDM_H
#define INC_OTA_BOARD_PDM_H

/*
 * Power Distribution Module OTA memory map (STM32U575CI, 2 MB, 8 KB pages, 768 KB SRAM,
 * TrustZone disabled).
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08008000, LENGTH = 2016K
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * Node ID 0x09 is not in MAIN_DBC yet. If the firmware keeps settings
 * in flash, lower OTA_BOARD_APP_END to the first page it uses.
 */

#include "ota_protocol.h"

#define OTA_BOARD_NODE_ID        OTA_NODE_PDM

#define OTA_BOARD_META_ADDR      0x08006000U   /* START marker + COMMIT record, own erase unit */
#define OTA_BOARD_APP_START      0x08008000U
#define OTA_BOARD_APP_END        0x08200000U   /* exclusive */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x200C0000U   /* SRAM1 + SRAM2 + SRAM3, 768 KB */

/* Bootloader: static const ota_layout_t layout = OTA_BOARD_LAYOUT; */
#define OTA_BOARD_LAYOUT {                       \
    .node_id   = OTA_BOARD_NODE_ID,           \
    .meta_addr = OTA_BOARD_META_ADDR,         \
    .app_start = OTA_BOARD_APP_START,         \
    .app_end   = OTA_BOARD_APP_END,           \
    .ram_start = OTA_BOARD_RAM_START,         \
    .ram_end   = OTA_BOARD_RAM_END,           \
}

#endif /* INC_OTA_BOARD_PDM_H */
