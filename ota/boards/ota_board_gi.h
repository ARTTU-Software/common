#ifndef INC_OTA_BOARD_GI_H
#define INC_OTA_BOARD_GI_H

/*
 * Galvanic Isolator (left/right) OTA memory map (STM32L562CE, 512 KB, TrustZone disabled).
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08007000, LENGTH = 484K
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * Addresses are 4 KB aligned so the map works with DBANK=1 (2 KB pages) and
 * DBANK=0 (4 KB pages).
 */

#include "ota_protocol.h"

/* Same firmware for both isolators: define OTA_BOARD_NODE_ID as OTA_NODE_GIL or
 * OTA_NODE_GIR before including this header (one bootloader build per side). */
#ifndef OTA_BOARD_NODE_ID
#error "Define OTA_BOARD_NODE_ID (OTA_NODE_GIL or OTA_NODE_GIR) before including ota_board_gi.h"
#endif

#define OTA_BOARD_META_ADDR      0x08006000U   /* START marker + COMMIT record, own erase unit */
#define OTA_BOARD_APP_START      0x08007000U
#define OTA_BOARD_APP_END        0x08080000U   /* exclusive */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x20040000U   /* SRAM1 192 KB + SRAM2 64 KB */

/* Bootloader: static const ota_layout_t layout = OTA_BOARD_LAYOUT; */
#define OTA_BOARD_LAYOUT {                       \
    .node_id   = OTA_BOARD_NODE_ID,           \
    .meta_addr = OTA_BOARD_META_ADDR,         \
    .app_start = OTA_BOARD_APP_START,         \
    .app_end   = OTA_BOARD_APP_END,           \
    .ram_start = OTA_BOARD_RAM_START,         \
    .ram_end   = OTA_BOARD_RAM_END,           \
}

#endif /* INC_OTA_BOARD_GI_H */
