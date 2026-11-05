#ifndef INC_OTA_BOARD_LVSOC_H
#define INC_OTA_BOARD_LVSOC_H

/*
 * LV SoC OTA memory map (STM32L5, TrustZone disabled).
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08007000, LENGTH = (flash size - 28K)
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * TODO(verify): the LV SoC repo was not available when this was written. Set
 * OTA_BOARD_APP_END below any flash it uses for NVM, and RAM_END to its SRAM.
 * Addresses are 4 KB aligned (works with DBANK=0 and DBANK=1).
 */

#include "ota_protocol.h"

#define OTA_BOARD_NODE_ID        OTA_NODE_LVSOC

#define OTA_BOARD_META_ADDR      0x08006000U   /* START marker + COMMIT record, own erase unit */
#define OTA_BOARD_APP_START      0x08007000U
#define OTA_BOARD_APP_END        0x08080000U   /* exclusive */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x20040000U   /* 256 KB; set to the part's SRAM end */

/* Bootloader: static const ota_layout_t layout = OTA_BOARD_LAYOUT; */
#define OTA_BOARD_LAYOUT {                       \
    .node_id   = OTA_BOARD_NODE_ID,           \
    .meta_addr = OTA_BOARD_META_ADDR,         \
    .app_start = OTA_BOARD_APP_START,         \
    .app_end   = OTA_BOARD_APP_END,           \
    .ram_start = OTA_BOARD_RAM_START,         \
    .ram_end   = OTA_BOARD_RAM_END,           \
}

#endif /* INC_OTA_BOARD_LVSOC_H */
