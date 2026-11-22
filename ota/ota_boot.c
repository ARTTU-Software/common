#include "ota_boot.h"
#include "ota_crc32.h"

#include <stddef.h>
#include <string.h>

#define OTA_RECORD_MAGIC_START   0x5341544FU /* "OTAS" */
#define OTA_RECORD_MAGIC_COMMIT  0x4341544FU /* "OTAC" */
#define OTA_RECORD_OFFSET_START  0x00U
#define OTA_RECORD_OFFSET_COMMIT 0x20U
#define OTA_BOOT_MAX_RECORD_WORDS 8U         /* covers program units up to 32 bytes */

/* 16 bytes: one H5 quad-word, two G4/L5 double-words. */
typedef struct {
    uint32_t magic;
    uint32_t size;
    uint32_t crc;
    uint32_t check; /* ~(magic ^ size ^ crc) */
} ota_record_t;

/* ── helpers ──────────────────────────────────────────────────── */

static void put_u32(uint8_t *dst, uint32_t v)
{
    dst[0] = (uint8_t)(v);
    dst[1] = (uint8_t)(v >> 8);
    dst[2] = (uint8_t)(v >> 16);
    dst[3] = (uint8_t)(v >> 24);
}

static uint32_t get_u32(const uint8_t *src)
{
    return (uint32_t)src[0] | ((uint32_t)src[1] << 8) | ((uint32_t)src[2] << 16) | ((uint32_t)src[3] << 24);
}

static uint32_t round_up(uint32_t v, uint32_t unit)
{
    return ((v + unit - 1U) / unit) * unit;
}

static uint32_t app_capacity(const ota_boot_t *ctx)
{
    return ctx->layout->app_end - ctx->layout->app_start;
}

static void reply(ota_boot_t *ctx, uint8_t op, uint8_t status, uint8_t b2, uint8_t b3, uint32_t value)
{
    uint8_t tx[8] = {op, status, b2, b3, 0U, 0U, 0U, 0U};

    put_u32(&tx[4], value);
    (void)ctx->port->can_send(OTA_ID_RSP(ctx->layout->node_id), tx, 8U);
}

/* ── records ──────────────────────────────────────────────────── */

static bool record_read(const ota_boot_t *ctx, uint32_t offset, uint32_t magic, ota_record_t *out)
{
    memcpy(out, ctx->port->flash_ptr(ctx->layout->meta_addr + offset), sizeof(*out));
    return (out->magic == magic) && (out->check == ~(out->magic ^ out->size ^ out->crc));
}

static bool record_write(const ota_boot_t *ctx, uint32_t offset, uint32_t magic, uint32_t size, uint32_t crc)
{
    /* Word array keeps the buffer aligned for the flash HAL. */
    uint32_t words[OTA_BOOT_MAX_RECORD_WORDS];
    ota_record_t rec = {magic, size, crc, ~(magic ^ size ^ crc)};
    uint32_t len = round_up((uint32_t)sizeof(rec), ctx->layout->program_unit);

    memset(words, 0xFF, sizeof(words));
    memcpy(words, &rec, sizeof(rec));
    return ctx->port->flash_program(ctx->layout->meta_addr + offset, (const uint8_t *)words, len);
}

/* ── image checks ─────────────────────────────────────────────── */

bool ota_boot_app_vector_valid(const ota_boot_t *ctx)
{
    const ota_layout_t *l = ctx->layout;
    const uint8_t *vt = ctx->port->flash_ptr(l->app_start);
    uint32_t sp = get_u32(&vt[0]);
    uint32_t reset = get_u32(&vt[4]);

    if ((sp < l->ram_start) || (sp > l->ram_end) || ((sp & 0x7U) != 0U)) {
        return false;
    }
    if ((reset < l->app_start) || (reset >= l->app_end) || ((reset & 1U) == 0U)) {
        return false;
    }
    return true;
}

uint32_t ota_boot_flags(const ota_boot_t *ctx)
{
    ota_record_t rec;
    uint32_t flags = 0U;
    bool started = record_read(ctx, OTA_RECORD_OFFSET_START, OTA_RECORD_MAGIC_START, &rec);
    bool committed = record_read(ctx, OTA_RECORD_OFFSET_COMMIT, OTA_RECORD_MAGIC_COMMIT, &rec);

    if (ota_boot_app_vector_valid(ctx)) {
        flags |= OTA_FLAG_APP_VECTOR_OK;
    }
    if (committed) {
        flags |= OTA_FLAG_COMMITTED;
    }
    if (started && !committed) {
        flags |= OTA_FLAG_UPDATE_INTERRUPTED;
    }
    return flags;
}

bool ota_boot_app_startable(const ota_boot_t *ctx)
{
    uint32_t flags = ota_boot_flags(ctx);

    if (((flags & OTA_FLAG_APP_VECTOR_OK) == 0U) || ((flags & OTA_FLAG_UPDATE_INTERRUPTED) != 0U)) {
        return false;
    }

#if OTA_BOOT_VERIFY_CRC_ON_BOOT
    if ((flags & OTA_FLAG_COMMITTED) != 0U) {
        ota_record_t rec;
        (void)record_read(ctx, OTA_RECORD_OFFSET_COMMIT, OTA_RECORD_MAGIC_COMMIT, &rec);
        if ((rec.size > app_capacity(ctx)) ||
            (ota_crc32(ctx->port->flash_ptr(ctx->layout->app_start), rec.size) != rec.crc)) {
            return false;
        }
    }
#endif

    return true;
}

/* ── command handlers ─────────────────────────────────────────── */

static void handle_info(ota_boot_t *ctx, uint8_t idx)
{
    ota_record_t rec;
    bool committed = record_read(ctx, OTA_RECORD_OFFSET_COMMIT, OTA_RECORD_MAGIC_COMMIT, &rec);
    uint32_t value;

    switch (idx) {
        case OTA_INFO_BL_VERSION:   value = (OTA_BOOT_VERSION_MAJOR << 16) | OTA_BOOT_VERSION_MINOR; break;
        case OTA_INFO_APP_START:    value = ctx->layout->app_start; break;
        case OTA_INFO_APP_MAX_SIZE: value = app_capacity(ctx); break;
        case OTA_INFO_APP_SIZE:     value = committed ? rec.size : 0U; break;
        case OTA_INFO_APP_CRC:      value = committed ? rec.crc : 0U; break;
        case OTA_INFO_FLAGS:        value = ota_boot_flags(ctx); break;
        default:
            reply(ctx, OTA_OP_INFO, OTA_ST_ERR_ARG, idx, 0U, 0U);
            return;
    }
    reply(ctx, OTA_OP_INFO, OTA_ST_OK, idx, 0U, value);
}

static void handle_start(ota_boot_t *ctx, uint32_t size)
{
    if ((size == 0U) || (size > app_capacity(ctx))) {
        reply(ctx, OTA_OP_START, OTA_ST_ERR_SIZE, 0U, 0U, size);
        return;
    }

    /* Invalidate the old image first: from here on, a reset keeps us in the bootloader. */
    if (!ctx->port->flash_erase(ctx->layout->meta_addr)) {
        reply(ctx, OTA_OP_START, OTA_ST_ERR_ERASE, 0U, 0U, size);
        return;
    }
    if (!record_write(ctx, OTA_RECORD_OFFSET_START, OTA_RECORD_MAGIC_START, size, 0U)) {
        reply(ctx, OTA_OP_START, OTA_ST_ERR_WRITE, 0U, 0U, size);
        return;
    }

    ctx->state = OTA_STATE_RECEIVING;
    ctx->image_size = size;
    ctx->bytes_written = 0U;
    ctx->erased_until = ctx->layout->app_start;
    ctx->next_seq = 0U;
    ctx->seq_error_sent = false;

    reply(ctx, OTA_OP_START, OTA_ST_OK, OTA_BLOCK_FRAMES, 0U, size);
}

static uint8_t write_block(ota_boot_t *ctx, uint32_t len)
{
    const ota_layout_t *l = ctx->layout;
    uint32_t addr = l->app_start + ctx->bytes_written;
    uint32_t padded = round_up(len, l->program_unit);

    /* Erase just ahead of the write pointer so no single command blocks for seconds. */
    while (ctx->erased_until < (addr + padded)) {
        if (!ctx->port->flash_erase(ctx->erased_until)) {
            return OTA_ST_ERR_ERASE;
        }
        ctx->erased_until += l->erase_unit;
    }

    if (!ctx->port->flash_program(addr, (const uint8_t *)ctx->block, padded)) {
        return OTA_ST_ERR_WRITE;
    }
    if (memcmp(ctx->port->flash_ptr(addr), ctx->block, padded) != 0) {
        return OTA_ST_ERR_VERIFY;
    }

    ctx->bytes_written += len;
    return OTA_ST_OK;
}

static void handle_data(ota_boot_t *ctx, const uint8_t *data, uint8_t len)
{
    uint32_t block_len;
    uint32_t frames_in_block;
    uint32_t offset;
    uint32_t n;
    uint8_t seq;

    if ((ctx->state != OTA_STATE_RECEIVING) || (ctx->bytes_written >= ctx->image_size)) {
        if (!ctx->seq_error_sent) {
            reply(ctx, OTA_OP_DATA, OTA_ST_ERR_STATE, 0U, 0U, ctx->bytes_written);
            ctx->seq_error_sent = true;
        }
        return;
    }

    seq = data[0];
    if (seq == 0U) {
        /* Start of a block, or the host restarting the current one. */
        ctx->next_seq = 0U;
        ctx->seq_error_sent = false;
        memset(ctx->block, 0xFF, sizeof(ctx->block));
    }

    if (seq != ctx->next_seq) {
        /* NACK once, then ignore frames until the host restarts the block at seq 0. */
        if (!ctx->seq_error_sent) {
            reply(ctx, OTA_OP_DATA, OTA_ST_ERR_SEQ, ctx->next_seq, 0U, ctx->bytes_written);
            ctx->seq_error_sent = true;
        }
        return;
    }

    block_len = ctx->image_size - ctx->bytes_written;
    if (block_len > OTA_BLOCK_BYTES) {
        block_len = OTA_BLOCK_BYTES;
    }
    frames_in_block = (block_len + OTA_DATA_BYTES_PER_FRAME - 1U) / OTA_DATA_BYTES_PER_FRAME;

    offset = (uint32_t)seq * OTA_DATA_BYTES_PER_FRAME;
    n = block_len - offset;
    if (n > OTA_DATA_BYTES_PER_FRAME) {
        n = OTA_DATA_BYTES_PER_FRAME;
    }
    if ((uint32_t)len < (1U + n)) {
        reply(ctx, OTA_OP_DATA, OTA_ST_ERR_ARG, seq, 0U, ctx->bytes_written);
        ctx->seq_error_sent = true;
        return;
    }

    memcpy((uint8_t *)ctx->block + offset, &data[1], n);
    ctx->next_seq++;

    if (ctx->next_seq == frames_in_block) {
        uint8_t st = write_block(ctx, block_len);

        ctx->next_seq = 0U;
        if (st != OTA_ST_OK) {
            /* Flash failure is not recoverable by retrying the block: drop the session. */
            ctx->state = OTA_STATE_IDLE;
        }
        reply(ctx, OTA_OP_DATA, st, 0U, 0U, ctx->bytes_written);
    }
}

static void handle_end(ota_boot_t *ctx, uint32_t crc)
{
    if (ctx->state != OTA_STATE_RECEIVING) {
        reply(ctx, OTA_OP_END, OTA_ST_ERR_STATE, 0U, 0U, 0U);
        return;
    }
    if (ctx->bytes_written != ctx->image_size) {
        reply(ctx, OTA_OP_END, OTA_ST_ERR_INCOMPLETE, 0U, 0U, ctx->bytes_written);
        return;
    }

    ctx->state = OTA_STATE_IDLE;

    if (ota_crc32(ctx->port->flash_ptr(ctx->layout->app_start), ctx->image_size) != crc) {
        reply(ctx, OTA_OP_END, OTA_ST_ERR_CRC, 0U, 0U, 0U);
        return;
    }
    if (!ota_boot_app_vector_valid(ctx)) {
        reply(ctx, OTA_OP_END, OTA_ST_ERR_VECTOR, 0U, 0U, 0U);
        return;
    }
    if (!record_write(ctx, OTA_RECORD_OFFSET_COMMIT, OTA_RECORD_MAGIC_COMMIT, ctx->image_size, crc)) {
        reply(ctx, OTA_OP_END, OTA_ST_ERR_WRITE, 0U, 0U, 0U);
        return;
    }

    reply(ctx, OTA_OP_END, OTA_ST_OK, 0U, 0U, crc);
}

static ota_boot_action_t handle_cmd(ota_boot_t *ctx, const uint8_t *data, uint8_t len)
{
    if (len < 1U) {
        return OTA_BOOT_ACTION_NONE;
    }

    ctx->host_seen = true;

    switch (data[0]) {
        case OTA_OP_PING: {
            uint8_t tx[8] = {OTA_OP_PING, OTA_ST_OK, OTA_MODE_BOOTLOADER, OTA_PROTOCOL_VERSION,
                             ctx->layout->node_id, 0U, 0U, 0U};
            (void)ctx->port->can_send(OTA_ID_RSP(ctx->layout->node_id), tx, 8U);
            break;
        }
        case OTA_OP_INFO:
            handle_info(ctx, (len >= 2U) ? data[1] : 0xFFU);
            break;
        case OTA_OP_START:
            if (len < 8U) {
                reply(ctx, OTA_OP_START, OTA_ST_ERR_ARG, 0U, 0U, 0U);
            } else {
                handle_start(ctx, get_u32(&data[4]));
            }
            break;
        case OTA_OP_END:
            if (len < 8U) {
                reply(ctx, OTA_OP_END, OTA_ST_ERR_ARG, 0U, 0U, 0U);
            } else {
                handle_end(ctx, get_u32(&data[4]));
            }
            break;
        case OTA_OP_STATUS:
            /* Also discards a partially received block: the host resumes from bytes_written. */
            ctx->next_seq = 0U;
            ctx->seq_error_sent = false;
            reply(ctx, OTA_OP_STATUS, OTA_ST_OK, ctx->state, 0U, ctx->bytes_written);
            break;
        case OTA_OP_ABORT:
            ctx->state = OTA_STATE_IDLE;
            reply(ctx, OTA_OP_ABORT, OTA_ST_OK, 0U, 0U, 0U);
            break;
        case OTA_OP_RESET:
            reply(ctx, OTA_OP_RESET, OTA_ST_OK, 0U, 0U, 0U);
            return OTA_BOOT_ACTION_RESET;
        default:
            reply(ctx, data[0], OTA_ST_ERR_UNKNOWN_OP, 0U, 0U, 0U);
            break;
    }
    return OTA_BOOT_ACTION_NONE;
}

/* ── public ───────────────────────────────────────────────────── */

void ota_boot_init(ota_boot_t *ctx, const ota_layout_t *layout, const ota_port_t *port)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->layout = layout;
    ctx->port = port;
    ctx->state = OTA_STATE_IDLE;
    ctx->last_activity_ms = port->tick_ms();
}

ota_boot_action_t ota_boot_poll(ota_boot_t *ctx)
{
    const uint8_t node = ctx->layout->node_id;
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
    uint32_t now;

    /* Bounded so timeouts are still evaluated under a frame flood. */
    for (uint32_t i = 0U; i < 16U; i++) {
        if (!ctx->port->can_recv(&id, data, &len)) {
            break;
        }

        if (id == OTA_ID_DATA(node)) {
            ctx->last_activity_ms = ctx->port->tick_ms();
            if (len >= 1U) {
                handle_data(ctx, data, len);
            }
        } else if (id == OTA_ID_CMD(node)) {
            ota_boot_action_t action;

            ctx->last_activity_ms = ctx->port->tick_ms();
            action = handle_cmd(ctx, data, len);
            if (action != OTA_BOOT_ACTION_NONE) {
                return action;
            }
        } else if ((id == OTA_ID_ENTER_REQUEST) && (len >= 1U) && (data[0] == node)) {
            /* Host asked an already-running bootloader to enter: just confirm. */
            ctx->host_seen = true;
            ctx->last_activity_ms = ctx->port->tick_ms();
            reply(ctx, OTA_OP_ENTER, OTA_ST_OK, 0U, 0U, 0U);
        }
    }

    now = ctx->port->tick_ms();
    if (ctx->state == OTA_STATE_RECEIVING) {
        if ((now - ctx->last_activity_ms) >= OTA_BOOT_SESSION_TIMEOUT_MS) {
            ctx->state = OTA_STATE_IDLE; /* START marker stays: app is not startable */
        }
    } else if ((now - ctx->last_activity_ms) >= OTA_BOOT_IDLE_TIMEOUT_MS) {
        ctx->last_activity_ms = now;
        if (ota_boot_app_startable(ctx)) {
            return OTA_BOOT_ACTION_JUMP_TO_APP;
        }
    }

    return OTA_BOOT_ACTION_NONE;
}
