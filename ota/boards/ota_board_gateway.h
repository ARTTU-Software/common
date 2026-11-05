#ifndef INC_OTA_BOARD_GATEWAY_H
#define INC_OTA_BOARD_GATEWAY_H

/*
 * CAN Gateway (front/rear) OTA memory map (STM32G431CB, 128 KB, 2 KB pages).
 * Included by both the board's application and its bootloader, so the
 * addresses can never disagree. Linker scripts must match:
 *   bootloader : FLASH ORIGIN = 0x08000000, LENGTH = 24K
 *   app        : FLASH ORIGIN = 0x08006800, LENGTH = 100K
 * The port takes erase/program units from the part and clamps the app end
 * to the real flash size.
 *
 * The last 2 KB page (0x0801F800) is the emulated EEPROM with the sensor
 * calibration: never touched by OTA. 100 KB remain for the application.
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
#define OTA_BOARD_APP_END        0x0801F800U   /* exclusive */

#define OTA_BOARD_RAM_START      0x20000000U
#define OTA_BOARD_RAM_END        0x20008000U   /* 32 KB (SRAM1 + SRAM2 + CCM alias) */

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
