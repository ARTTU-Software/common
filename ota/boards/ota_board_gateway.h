#ifndef INC_OTA_BOARD_GATEWAY_H
#define INC_OTA_BOARD_GATEWAY_H

/*
 * CAN Gateway (front/rear) OTA memory map.
 * STM32G473CE: 512 KB flash in dual-bank mode (2 KB pages), 128 KB RAM, CAN on FDCAN2.
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08006800, LENGTH = 100K
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * The app area stops at 0x0801F800 on purpose: the gateway's emulated EEPROM
 * (sensor calibration) is configured in Drivers/Flash_EE/ee_config.h as page 63
 * of bank 1 = 0x0801F800, in the middle of the 512 KB flash. OTA never touches
 * it. 100 KB remain for the application (64-67 KB used, Debug build, 2026-10).
 * To use the rest of the flash, move the EEPROM page to the end of bank 2
 * first and raise OTA_BOARD_APP_END; that loses the stored calibration.
 * The page-63 EEPROM setting only works with DBANK=1 (the factory default).
 */

#include "ota_protocol.h"

/* Same firmware for both gateways: define OTA_BOARD_NODE_ID as OTA_NODE_GF or
 * OTA_NODE_GR before including this header (app: from DEVICE_TYPE; bootloader:
 * from the CMake preset, one bootloader build per gateway). */
#ifndef OTA_BOARD_NODE_ID
#error "Define OTA_BOARD_NODE_ID (OTA_NODE_GF or OTA_NODE_GR) before including ota_board_gateway.h"
#endif

#define OTA_BOARD_META_ADDR      0x08006000U   /* START marker + COMMIT record, own erase unit */
#define OTA_BOARD_APP_START      0x08006800U
#define OTA_BOARD_APP_END        0x0801F800U   /* exclusive: the EEPROM page starts here */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x20020000U   /* 128 KB (SRAM1 + SRAM2 + CCM alias) */

/* Bootloader: static const ota_layout_t layout = OTA_BOARD_LAYOUT; */
#define OTA_BOARD_LAYOUT {                       \
    .node_id   = OTA_BOARD_NODE_ID,           \
    .meta_addr = OTA_BOARD_META_ADDR,         \
    .app_start = OTA_BOARD_APP_START,         \
    .app_end   = OTA_BOARD_APP_END,           \
    .ram_start = OTA_BOARD_RAM_START,         \
    .ram_end   = OTA_BOARD_RAM_END,           \
}

#endif /* INC_OTA_BOARD_GATEWAY_H */
