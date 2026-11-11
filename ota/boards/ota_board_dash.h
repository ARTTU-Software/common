#ifndef INC_OTA_BOARD_DASH_H
#define INC_OTA_BOARD_DASH_H

/*
 * Dashboard OTA memory map (STM32H563ZI, 2 MB, 8 KB sectors, 640 KB RAM).
 * The older dashboard (H563RG, 1 MB) used FDCAN2: pass whichever FDCAN the new
 * board uses to ota_app_stm32_init()/ota_bootloader_run().
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08008000, LENGTH = 2000K
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * TODO(verify against the Dashboard repo, not available when this was written):
 *  - Flash config (Drivers/Flash_EE, NimaLTD ee). With the library's automatic
 *    configuration it sits in the last sector of bank 2 = 0x081FE000 on this
 *    2 MB part; the last 16 KB (2 sectors) are kept free here to cover that.
 *    If ee_config.h uses EE_MANUAL_CONFIG, set OTA_BOARD_APP_END to the lowest
 *    address it uses.
 *  - Images above 992 KB cross into flash bank 2, which the H5 port handles
 *    but which has not been exercised on hardware yet.
 */

#include "ota_protocol.h"

#define OTA_BOARD_NODE_ID        OTA_NODE_DASH

#define OTA_BOARD_META_ADDR      0x08006000U   /* START marker + COMMIT record, own erase unit */
#define OTA_BOARD_APP_START      0x08008000U
#define OTA_BOARD_APP_END        0x081FC000U   /* exclusive: last 16 KB kept for the flash config */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x200A0000U   /* 640 KB */

/* Bootloader: static const ota_layout_t layout = OTA_BOARD_LAYOUT; */
#define OTA_BOARD_LAYOUT {                       \
    .node_id   = OTA_BOARD_NODE_ID,           \
    .meta_addr = OTA_BOARD_META_ADDR,         \
    .app_start = OTA_BOARD_APP_START,         \
    .app_end   = OTA_BOARD_APP_END,           \
    .ram_start = OTA_BOARD_RAM_START,         \
    .ram_end   = OTA_BOARD_RAM_END,           \
}

#endif /* INC_OTA_BOARD_DASH_H */
