#include "generic_sensor_recal.h"
#include <stdbool.h>
#include <string.h>

// Recal_Instance is defined in the public header so callers can allocate it.

// Simple CRC32 (bitwise) for portability
static uint32_t crc32_calc(const void *buf, size_t len)
{
    const uint8_t *p = (const uint8_t*)buf;
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc ^= p[i];
        for (int j = 0; j < 8; ++j) {
            uint32_t mask = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}

Recal_Instance_t *Recal_Init(Recal_Instance_t *inst,
                             Recal_Callbacks_t const *cb,
                             Recal_Config_t const *config)
{
    if (!inst || !cb || !cb->read_raw || !config) return NULL;
    memset(inst, 0, sizeof(*inst));
    inst->cb = *cb;
    inst->state = RECAL_STATE_IDLE;
    inst->sample_window = config->sample_window ? config->sample_window : 16;
    inst->min_offset = config->min_offset;
    inst->max_offset = config->max_offset;
    inst->last_error = RECAL_OK;
    inst->data.version = 1;
    inst->data.offset = 0.0f;
    inst->data.scale = 1.0f;
    return inst;
}

int Recal_Start(Recal_Instance_t *inst)
{
    if (!inst) return RECAL_ERR_INVALID_ARG;
    if (inst->state != RECAL_STATE_IDLE && inst->state != RECAL_STATE_COMPLETED) {
        return RECAL_ERR_INVALID_ARG;
    }
    inst->have_min = false;
    inst->sample_acc = 0;
    inst->sample_count = 0;
    inst->measured_min = 0;
    inst->measured_max = 0;
    inst->last_error = RECAL_OK;
    inst->state = RECAL_STATE_MEASURE_MIN_SAMPLING;
    return RECAL_OK;
}

int Recal_Capture(Recal_Instance_t *inst)
{
    if (!inst) return RECAL_ERR_INVALID_ARG;
    if (inst->state != RECAL_STATE_WAIT_MAX_CAPTURE) return RECAL_ERR_INVALID_ARG;
    inst->sample_acc = 0;
    inst->sample_count = 0;
    inst->state = RECAL_STATE_MEASURE_MAX_SAMPLING;
    return RECAL_OK;
}

int Recal_Abort(Recal_Instance_t *inst)
{
    if (!inst) return RECAL_ERR_INVALID_ARG;
    inst->state = RECAL_STATE_IDLE;
    return RECAL_OK;
}

static void sampling_step(Recal_Instance_t *inst)
{
    uint32_t sample = inst->cb.read_raw(inst->cb.context);
    inst->sample_acc += sample;
    inst->sample_count++;
    if (inst->sample_count >= inst->sample_window) {
        uint32_t avg = (uint32_t)(inst->sample_acc / inst->sample_count);
        inst->sample_acc = 0;
        inst->sample_count = 0;
        if (inst->state == RECAL_STATE_MEASURE_MIN_SAMPLING) {
            inst->measured_min = avg;
            inst->have_min = true;
            inst->state = RECAL_STATE_WAIT_MAX_CAPTURE;
        } else if (inst->state == RECAL_STATE_MEASURE_MAX_SAMPLING) {
            inst->measured_max = avg;
            inst->state = RECAL_STATE_COMPUTE;
        }
    }
}

void Recal_Tick(Recal_Instance_t *inst, uint32_t dt_ms)
{
    (void)dt_ms;
    if (!inst) return;
    switch (inst->state) {
        case RECAL_STATE_MEASURE_MIN_SAMPLING:
        case RECAL_STATE_MEASURE_MAX_SAMPLING:
            sampling_step(inst);
            break;

        case RECAL_STATE_COMPUTE: {
            uint32_t adjusted_min = inst->measured_min + (uint32_t)inst->min_offset;
            uint32_t adjusted_max = inst->measured_max;
            if (adjusted_max > inst->max_offset) {
                adjusted_max -= (uint32_t)inst->max_offset;
            } else {
                inst->last_error = RECAL_ERR_PLAUSIBLE;
                inst->state = RECAL_STATE_ERROR;
                break;
            }

            if (adjusted_max <= adjusted_min) {
                inst->last_error = RECAL_ERR_PLAUSIBLE;
                inst->state = RECAL_STATE_ERROR;
                break;
            }

            float denom = (float)(adjusted_max - adjusted_min);
            inst->data.raw_min = adjusted_min;
            inst->data.raw_max = adjusted_max;
            inst->data.scale = 1.0f / denom;
            inst->data.offset = -((float)adjusted_min) * inst->data.scale;
            inst->state = RECAL_STATE_STORE;
            break;
        }

        case RECAL_STATE_STORE: {
            if (inst->cb.nvm_write == NULL || inst->cb.nvm_read == NULL) {
                inst->state = RECAL_STATE_COMPLETED;
            } else {
                int rc = Recal_SaveToNvm(inst);
                if (rc != 0) {
                    inst->last_error = RECAL_ERR_NVM;
                    inst->state = RECAL_STATE_ERROR;
                } else {
                    inst->state = RECAL_STATE_VERIFY;
                }
            }
            break;
        }

        case RECAL_STATE_VERIFY: {
            int rc = Recal_LoadFromNvm(inst);
            if (rc != 0) {
                inst->last_error = RECAL_ERR_NVM;
                inst->state = RECAL_STATE_ERROR;
            } else {
                inst->state = RECAL_STATE_COMPLETED;
            }
            break;
        }

        default:
            break;
    }
}

Recal_State_t Recal_GetState(Recal_Instance_t *inst)
{
    if (!inst) return RECAL_STATE_ERROR;
    return inst->state;
}

const Recal_Data_t *Recal_GetData(Recal_Instance_t *inst)
{
    if (!inst) return NULL;
    return &inst->data;
}

int Recal_SaveToNvm(Recal_Instance_t *inst)
{
    if (!inst || !inst->cb.nvm_write) return RECAL_ERR_NVM;
    // compute crc over fields except crc
    Recal_Data_t tmp = inst->data;
    tmp.crc = 0;
    tmp.crc = crc32_calc(&tmp, offsetof(Recal_Data_t, crc));
    // write tmp (including crc)
    int rc = inst->cb.nvm_write(&tmp, sizeof(tmp), inst->cb.context);
    if (rc != 0) return RECAL_ERR_NVM;
    // copy back
    inst->data = tmp;
    return 0;
}

int Recal_LoadFromNvm(Recal_Instance_t *inst)
{
    if (!inst || !inst->cb.nvm_read) return RECAL_ERR_NVM;
    Recal_Data_t tmp;
    int rc = inst->cb.nvm_read(&tmp, sizeof(tmp), inst->cb.context);
    if (rc != 0) return RECAL_ERR_NVM;
    uint32_t crc = tmp.crc;
    tmp.crc = 0;
    uint32_t calc = crc32_calc(&tmp, offsetof(Recal_Data_t, crc));
    if (calc != crc) return RECAL_ERR_NVM;
    // successful load
    tmp.crc = crc;
    inst->data = tmp;
    return 0;
}
