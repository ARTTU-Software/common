#ifndef INC_OTA_APP_H
#define INC_OTA_APP_H

/*
 * Application-side OTA hook.
 *
 * Lives in the running firmware. Answers PING (so the host can discover
 * nodes) and handles the 0x4F0 enter-bootloader request. HAL-free: the
 * board supplies the callbacks below.
 *
 * Integration:
 *   1. Let OTA_ID_ENTER_REQUEST and OTA_ID_CMD(node) through the CAN filters.
 *   2. Call ota_app_init() once after CAN is up.
 *   3. Pass every received frame (or just those two IDs) to
 *      ota_app_handle_frame() from main-loop context, never from an ISR,
 *      because a granted request does not return.
 */

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t node_id;

    /** Queue one classic CAN frame. Return true on success. */
    bool (*send)(uint32_t id, const uint8_t *data, uint8_t len);

    /**
     * Return true when it is safe to drop out of the application
     * (e.g. tractive system off). NULL means always safe.
     */
    bool (*is_safe_to_update)(void);

    /**
     * Wait for the TX FIFO to drain, set the bootloader flag
     * (OTA_BOOT_FLAG_MAGIC in the backup register) and reset. Must not return.
     */
    void (*enter_bootloader)(void);
} ota_app_config_t;

void ota_app_init(const ota_app_config_t *cfg);

/**
 * Feed a received frame to the OTA hook.
 * @return true if the frame was an OTA frame for this node and was consumed.
 */
bool ota_app_handle_frame(uint32_t id, const uint8_t *data, uint8_t len);

#endif /* INC_OTA_APP_H */
