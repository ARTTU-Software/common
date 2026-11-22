#include "ota_app.h"
#include "ota_protocol.h"

#include <stddef.h>

static const ota_app_config_t *ota_cfg = NULL;

static void ota_app_reply(uint8_t op, uint8_t status, uint8_t b2, uint8_t b3)
{
    uint8_t tx[8] = {op, status, b2, b3, ota_cfg->node_id, 0U, 0U, 0U};

    (void)ota_cfg->send(OTA_ID_RSP(ota_cfg->node_id), tx, 8U);
}

static bool ota_app_request_is_for_us(const uint8_t *data, uint8_t len)
{
    return (len >= 4U) &&
           (data[0] == ota_cfg->node_id) &&
           (data[1] == (uint8_t)OTA_ENTER_KEY_0) &&
           (data[2] == (uint8_t)OTA_ENTER_KEY_1) &&
           (data[3] == (uint8_t)OTA_ENTER_KEY_2);
}

void ota_app_init(const ota_app_config_t *cfg)
{
    ota_cfg = cfg;
}

bool ota_app_handle_frame(uint32_t id, const uint8_t *data, uint8_t len)
{
    if ((ota_cfg == NULL) || (ota_cfg->send == NULL) || (data == NULL)) {
        return false;
    }

    if (id == OTA_ID_ENTER_REQUEST) {
        if (!ota_app_request_is_for_us(data, len)) {
            return false; /* request for another node, or legacy frame */
        }

        if ((ota_cfg->is_safe_to_update != NULL) && !ota_cfg->is_safe_to_update()) {
            ota_app_reply(OTA_OP_ENTER, OTA_ST_ERR_NOT_SAFE, 0U, 0U);
            return true;
        }

        ota_app_reply(OTA_OP_ENTER, OTA_ST_OK, 0U, 0U);
        if (ota_cfg->enter_bootloader != NULL) {
            ota_cfg->enter_bootloader(); /* does not return */
        }
        return true;
    }

    if (id == OTA_ID_CMD(ota_cfg->node_id)) {
        if ((len >= 1U) && (data[0] == OTA_OP_PING)) {
            ota_app_reply(OTA_OP_PING, OTA_ST_OK, OTA_MODE_APP, OTA_PROTOCOL_VERSION);
        } else {
            /* Everything else needs the bootloader. */
            ota_app_reply((len >= 1U) ? data[0] : 0U, OTA_ST_ERR_STATE, 0U, 0U);
        }
        return true;
    }

    return false;
}
