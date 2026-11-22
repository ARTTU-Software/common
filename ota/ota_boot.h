#ifndef INC_OTA_BOOT_H
#define INC_OTA_BOOT_H

/*
 * Bootloader-side OTA core: session state machine, image validation and
 * commit records. HAL-free; all hardware access goes through ota_port_t,
 * implemented once per MCU family (port/ota_port_stm32h5.c, ...).
 *
 * Flash layout handled here:
 *
 *   meta_addr   +0x00  START marker  (written when an update begins)
 *               +0x20  COMMIT record (written after the image CRC verified)
 *   app_start   application image (vector table first)
 *   app_end     end of the area the bootloader may touch (exclusive)
 *
 * The meta area must be its own erase unit, outside the app area.
 */

#include <stdbool.h>
#include <stdint.h>

#include "ota_protocol.h"

#define OTA_BOOT_VERSION_MAJOR  2U
#define OTA_BOOT_VERSION_MINOR  0U

/* Abort a stalled transfer after this long without a frame (ms). */
#ifndef OTA_BOOT_SESSION_TIMEOUT_MS
#define OTA_BOOT_SESSION_TIMEOUT_MS   10000U
#endif

/* With a valid app and no update in progress, jump to it after this long idle (ms). */
#ifndef OTA_BOOT_IDLE_TIMEOUT_MS
#define OTA_BOOT_IDLE_TIMEOUT_MS      30000U
#endif

/*
 * Re-check the whole-image CRC on every boot. Off by default: the COMMIT
 * record is only written after the CRC was verified, and enabling this means
 * an app flashed with a debugger (no matching commit) will not start.
 */
#ifndef OTA_BOOT_VERIFY_CRC_ON_BOOT
#define OTA_BOOT_VERIFY_CRC_ON_BOOT   0
#endif

typedef struct {
    /* CAN, non-blocking */
    bool (*can_send)(uint32_t id, const uint8_t *data, uint8_t len);
    bool (*can_recv)(uint32_t *id, uint8_t *data, uint8_t *len);

    uint32_t (*tick_ms)(void);

    /* Flash. addr is a CPU address. */
    bool (*flash_erase)(uint32_t addr);   /* erase the single erase unit starting at addr */
    bool (*flash_program)(uint32_t addr, const uint8_t *data, uint32_t len); /* len multiple of program_unit */
    const uint8_t *(*flash_ptr)(uint32_t addr); /* readable pointer to addr (memory-mapped on target) */
} ota_port_t;

typedef struct {
    uint8_t  node_id;
    uint32_t meta_addr;
    uint32_t app_start;       /* must be erase_unit aligned */
    uint32_t app_end;         /* exclusive */
    uint32_t erase_unit;
    uint32_t program_unit;    /* 8 or 16, must divide OTA_BLOCK_BYTES */
    uint32_t ram_start;       /* for the initial-SP plausibility check */
    uint32_t ram_end;         /* exclusive */
} ota_layout_t;

typedef enum {
    OTA_BOOT_ACTION_NONE = 0,
    OTA_BOOT_ACTION_JUMP_TO_APP,
    OTA_BOOT_ACTION_RESET,
} ota_boot_action_t;

typedef struct {
    const ota_layout_t *layout;
    const ota_port_t   *port;

    uint8_t  state;             /* OTA_STATE_* */
    uint32_t image_size;
    uint32_t bytes_written;
    uint32_t erased_until;      /* app area erased in [app_start, erased_until) */
    uint8_t  next_seq;          /* expected DATA seq inside the current block */
    bool     seq_error_sent;    /* NACK already sent, waiting for host to resync */
    uint32_t last_activity_ms;
    bool     host_seen;         /* any valid CMD received since boot */

    uint32_t block[OTA_BLOCK_BYTES / 4U]; /* word-aligned for the flash HAL */
} ota_boot_t;

void ota_boot_init(ota_boot_t *ctx, const ota_layout_t *layout, const ota_port_t *port);

/** Vector table at app_start looks like a real image (SP in RAM, reset handler in app area, Thumb bit). */
bool ota_boot_app_vector_valid(const ota_boot_t *ctx);

/**
 * Boot decision. True when the app may be started right away:
 * vector table plausible and no interrupted update (START marker without COMMIT).
 */
bool ota_boot_app_startable(const ota_boot_t *ctx);

/** Bitfield of OTA_FLAG_* describing the installed image. */
uint32_t ota_boot_flags(const ota_boot_t *ctx);

/**
 * Process pending CAN frames and timeouts. Call continuously.
 * The caller performs the returned action (jump / reset).
 */
ota_boot_action_t ota_boot_poll(ota_boot_t *ctx);

#endif /* INC_OTA_BOOT_H */
