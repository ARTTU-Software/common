#ifndef INC_OTA_PROTOCOL_H
#define INC_OTA_PROTOCOL_H

/*
 * OTA-over-CAN protocol definitions.
 *
 * Shared by the application hook (ota_app), the bootloader core (ota_boot)
 * and the host tool (ota/tools/ota_flash.py mirrors these values).
 * HAL-free on purpose so it can move to common/ unchanged.
 *
 * All frames are classic CAN, standard 11-bit IDs, 8 data bytes.
 * Multi-byte fields are little-endian.
 *
 * ID scheme (0x600-0x6FF is unused by MAIN_DBC):
 *   0x4F0           PRG_Bootloader   host -> all    enter-bootloader request (existing DBC message)
 *   0x6N0           CMD              host -> node   [op, args...]
 *   0x6N1           RSP              node -> host   [op, status, ...]
 *   0x6N2           DATA             host -> node   [seq, 7 bytes of image]
 *   N = node id (DBC board names below)
 */

#include <stdint.h>

#define OTA_PROTOCOL_VERSION        2U

/* ── Node IDs, aligned with MAIN_DBC board names ─────────────── */
#define OTA_NODE_ECU                0x01U
#define OTA_NODE_GF                 0x02U   /* Gateway Front (matches legacy 0x4F0 payload) */
#define OTA_NODE_GR                 0x03U   /* Gateway Rear  (matches legacy 0x4F0 payload) */
#define OTA_NODE_GIL                0x04U   /* Galvanic Isolator Left  */
#define OTA_NODE_GIR                0x05U   /* Galvanic Isolator Right */
#define OTA_NODE_DASH               0x06U   /* Dashboard */
#define OTA_NODE_TELEM              0x07U   /* Telemetry */
#define OTA_NODE_LVSOC              0x08U   /* LV SoC */

/* ── CAN IDs ──────────────────────────────────────────────────── */
#define OTA_ID_ENTER_REQUEST        0x4F0U  /* == CAN_MSG_PRG_BOOTLOADER_ID */
#define OTA_ID_BASE                 0x600U
#define OTA_ID_CMD(n)               (OTA_ID_BASE | ((uint32_t)(n) << 4) | 0x0U)
#define OTA_ID_RSP(n)               (OTA_ID_BASE | ((uint32_t)(n) << 4) | 0x1U)
#define OTA_ID_DATA(n)              (OTA_ID_BASE | ((uint32_t)(n) << 4) | 0x2U)

/* 0x4F0 payload: [node_id, 'O', 'T', 'A', 0, 0, 0, 0] */
#define OTA_ENTER_KEY_0             'O'
#define OTA_ENTER_KEY_1             'T'
#define OTA_ENTER_KEY_2             'A'

/* ── Opcodes (CMD byte 0, echoed in RSP byte 0) ───────────────── */
#define OTA_OP_PING                 0x01U   /* -> [op, st, mode, proto, node, 0, 0, 0]             */
#define OTA_OP_INFO                 0x02U   /* [op, idx] -> [op, st, idx, 0, u32 value]            */
#define OTA_OP_START                0x03U   /* [op, 0, 0, 0, u32 size] -> [op, st, frames/block, 0, u32 size] */
#define OTA_OP_DATA                 0x04U   /* RSP only, per block: [op, st, 0, 0, u32 bytes_written] */
#define OTA_OP_END                  0x05U   /* [op, 0, 0, 0, u32 crc32] -> [op, st]                */
#define OTA_OP_ABORT                0x06U   /* -> [op, st]                                         */
#define OTA_OP_RESET                0x07U   /* -> [op, st], then the node resets                   */
#define OTA_OP_STATUS               0x08U   /* -> [op, st, state, 0, u32 bytes_written]            */
#define OTA_OP_ENTER                0x10U   /* RSP only, answer to 0x4F0: [op, st]                 */

/* ── INFO indexes ─────────────────────────────────────────────── */
#define OTA_INFO_BL_VERSION         0x00U   /* (major << 16) | minor                     */
#define OTA_INFO_APP_START          0x01U
#define OTA_INFO_APP_MAX_SIZE       0x02U
#define OTA_INFO_APP_SIZE           0x03U   /* from commit record, 0 if none             */
#define OTA_INFO_APP_CRC            0x04U   /* from commit record, 0 if none             */
#define OTA_INFO_FLAGS              0x05U   /* OTA_FLAG_* bitfield                       */

#define OTA_FLAG_APP_VECTOR_OK      (1U << 0)
#define OTA_FLAG_COMMITTED          (1U << 1)
#define OTA_FLAG_UPDATE_INTERRUPTED (1U << 2)

/* ── PING mode byte ───────────────────────────────────────────── */
#define OTA_MODE_APP                0x00U
#define OTA_MODE_BOOTLOADER         0x01U

/* ── Bootloader session state (STATUS byte 2) ─────────────────── */
#define OTA_STATE_IDLE              0x00U
#define OTA_STATE_RECEIVING         0x01U

/* ── Status / error codes (RSP byte 1) ────────────────────────── */
#define OTA_ST_OK                   0x00U
#define OTA_ST_ERR_STATE            0x01U   /* command not valid in current state   */
#define OTA_ST_ERR_SIZE             0x02U   /* image size 0 or larger than app area */
#define OTA_ST_ERR_ERASE            0x03U
#define OTA_ST_ERR_WRITE            0x04U
#define OTA_ST_ERR_VERIFY           0x05U   /* read-back after program mismatched   */
#define OTA_ST_ERR_CRC              0x06U   /* whole-image CRC mismatch             */
#define OTA_ST_ERR_VECTOR           0x07U   /* image vector table not plausible     */
#define OTA_ST_ERR_SEQ              0x08U   /* DATA frame out of sequence           */
#define OTA_ST_ERR_NOT_SAFE         0x09U   /* app refused: vehicle not in safe state */
#define OTA_ST_ERR_UNKNOWN_OP       0x0AU
#define OTA_ST_ERR_ARG              0x0BU
#define OTA_ST_ERR_INCOMPLETE       0x0CU   /* END before all bytes were received   */

/* ── Transfer parameters ──────────────────────────────────────── */
#define OTA_DATA_BYTES_PER_FRAME    7U
#define OTA_BLOCK_FRAMES            64U     /* frames per acknowledged block            */
#define OTA_BLOCK_BYTES             (OTA_BLOCK_FRAMES * OTA_DATA_BYTES_PER_FRAME) /* 448 */

/* ── App -> bootloader handshake (backup register) ────────────── */
#define OTA_BOOT_FLAG_MAGIC         0xDEADC0DEU

#endif /* INC_OTA_PROTOCOL_H */
