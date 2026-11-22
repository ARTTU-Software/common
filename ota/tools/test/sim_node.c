/*
 * Host-side simulated OTA node: the real ota_boot.c / ota_app.c running on
 * emulated STM32H5-like flash. Built as a shared library and driven from
 * test_ota_sim.py through ctypes, so the real host client talks to the real
 * bootloader core.
 *
 * Flash rules emulated: 8 KB erase units, 16-byte program unit, programming a
 * non-erased location fails (like H5 ECC-protected flash).
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ota_app.h"
#include "ota_boot.h"
#include "ota_protocol.h"

#if defined(_WIN32)
#define SIM_API __declspec(dllexport)
#else
#define SIM_API __attribute__((visibility("default")))
#endif

#define SIM_NODE        OTA_NODE_ECU
#define SIM_META        0x08006000U
#define SIM_APP_START   0x08008000U
#define SIM_APP_SIZE    (256U * 1024U)
#define SIM_APP_END     (SIM_APP_START + SIM_APP_SIZE)
#define SIM_ERASE_UNIT  0x2000U
#define SIM_PROG_UNIT   16U
#define SIM_QUEUE_LEN   256U

typedef struct { uint32_t id; uint8_t len; uint8_t data[8]; } sim_frame_t;
typedef struct { sim_frame_t f[SIM_QUEUE_LEN]; uint32_t head, tail; } sim_queue_t;

static uint8_t sim_flash[SIM_APP_END - SIM_META];
static sim_queue_t rxq, txq;
static uint32_t sim_tick;
static bool boot_flag;
static int mode; /* OTA_MODE_APP / OTA_MODE_BOOTLOADER */
static bool pending_reset;
static bool listening;          /* inside the post-reset listen window */
static uint32_t listen_until;
static int fail_program_after = -1;

static ota_boot_t ctx;

static bool q_push(sim_queue_t *q, uint32_t id, const uint8_t *d, uint8_t len)
{
    uint32_t next = (q->head + 1U) % SIM_QUEUE_LEN;
    if (next == q->tail) {
        return false;
    }
    q->f[q->head].id = id;
    q->f[q->head].len = len;
    memset(q->f[q->head].data, 0, 8);
    memcpy(q->f[q->head].data, d, len);
    q->head = next;
    return true;
}

static bool q_pop(sim_queue_t *q, uint32_t *id, uint8_t *d, uint8_t *len)
{
    if (q->head == q->tail) {
        return false;
    }
    *id = q->f[q->tail].id;
    *len = q->f[q->tail].len;
    memcpy(d, q->f[q->tail].data, 8);
    q->tail = (q->tail + 1U) % SIM_QUEUE_LEN;
    return true;
}

/* ── port ─────────────────────────────────────────────────────── */

static bool p_send(uint32_t id, const uint8_t *d, uint8_t len) { return q_push(&txq, id, d, len); }
static bool p_recv(uint32_t *id, uint8_t *d, uint8_t *len) { return q_pop(&rxq, id, d, len); }
static uint32_t p_tick(void) { return sim_tick; }

static bool in_flash(uint32_t addr, uint32_t len)
{
    return (addr >= SIM_META) && (addr + len <= SIM_APP_END) && (addr + len >= addr);
}

static bool p_erase(uint32_t addr)
{
    if (((addr - SIM_META) % SIM_ERASE_UNIT) != 0U || !in_flash(addr, SIM_ERASE_UNIT)) {
        return false;
    }
    memset(&sim_flash[addr - SIM_META], 0xFF, SIM_ERASE_UNIT);
    return true;
}

static bool p_program(uint32_t addr, const uint8_t *d, uint32_t len)
{
    if ((addr % SIM_PROG_UNIT) != 0U || (len % SIM_PROG_UNIT) != 0U || !in_flash(addr, len)) {
        return false;
    }
    for (uint32_t i = 0; i < len; i++) {
        if (sim_flash[addr - SIM_META + i] != 0xFF) {
            return false; /* no overwrite without erase */
        }
    }
    if (fail_program_after == 0) {
        return false;
    }
    if (fail_program_after > 0) {
        fail_program_after--;
    }
    memcpy(&sim_flash[addr - SIM_META], d, len);
    return true;
}

static const uint8_t *p_ptr(uint32_t addr) { return &sim_flash[addr - SIM_META]; }

static const ota_port_t port = {p_send, p_recv, p_tick, p_erase, p_program, p_ptr};

static const ota_layout_t layout = {
    .node_id = SIM_NODE,
    .meta_addr = SIM_META,
    .app_start = SIM_APP_START,
    .app_end = SIM_APP_END,
    .erase_unit = SIM_ERASE_UNIT,
    .program_unit = SIM_PROG_UNIT,
    .ram_start = 0x20000000U,
    .ram_end = 0x200A0000U,
};

/* ── app side ─────────────────────────────────────────────────── */

static bool app_safe = true;
static bool app_is_safe(void) { return app_safe; }
static void app_enter_bootloader(void) { boot_flag = true; pending_reset = true; }
static const ota_app_config_t app_cfg = {SIM_NODE, p_send, app_is_safe, app_enter_bootloader};

/* ── API ──────────────────────────────────────────────────────── */

#define SIM_LISTEN_MS 50U /* OTA_BOOT_LISTEN_MS */

static void enter_app(void)
{
    listening = false;
    mode = OTA_MODE_APP;
    ota_app_init(&app_cfg);
}

/* Power-on / reset: same decision as ota_bootloader_run(), including the listen window. */
SIM_API void sim_reset(void)
{
    bool requested = boot_flag;

    boot_flag = false;
    rxq.head = rxq.tail = 0U;
    ota_boot_init(&ctx, &layout, &port);
    mode = OTA_MODE_BOOTLOADER;
    listening = !requested && ota_boot_app_startable(&ctx);
    listen_until = sim_tick + SIM_LISTEN_MS;
}

/* End of the listen window: a host that spoke keeps us in the bootloader, otherwise start the app. */
static void listen_check(void)
{
    if (!listening) {
        return;
    }
    if (ctx.host_seen) {
        listening = false;
    } else if ((int32_t)(sim_tick - listen_until) >= 0) {
        enter_app();
    }
}

SIM_API void sim_power_on_blank(void)
{
    memset(sim_flash, 0xFF, sizeof(sim_flash));
    txq.head = txq.tail = 0U;
    sim_tick = 0U;
    fail_program_after = -1;
    app_safe = true;
    sim_reset();
}

static void run_boot(void)
{
    for (int i = 0; i < 64; i++) {
        ota_boot_action_t a = ota_boot_poll(&ctx);
        if (a == OTA_BOOT_ACTION_RESET) {
            sim_reset();
            return;
        }
        if (a == OTA_BOOT_ACTION_JUMP_TO_APP) {
            enter_app();
            return;
        }
        if (rxq.head == rxq.tail) {
            break;
        }
    }
    listen_check();
}

SIM_API void sim_host_send(uint32_t id, const uint8_t *d, uint8_t len)
{
    if (mode == OTA_MODE_BOOTLOADER) {
        (void)q_push(&rxq, id, d, len);
        run_boot();
    } else {
        (void)ota_app_handle_frame(id, d, len);
        if (pending_reset) {
            pending_reset = false;
            sim_reset();
        }
    }
}

SIM_API int sim_host_recv(uint32_t *id, uint8_t *d, uint8_t *len) { return q_pop(&txq, id, d, len) ? 1 : 0; }

SIM_API void sim_advance(uint32_t ms)
{
    sim_tick += ms;
    if (mode == OTA_MODE_BOOTLOADER) {
        run_boot();
    }
}

SIM_API int sim_listening(void) { return listening ? 1 : 0; }

SIM_API int sim_mode(void) { return mode; }
SIM_API uint32_t sim_flags(void) { return ota_boot_flags(&ctx); }
SIM_API int sim_app_startable(void) { return ota_boot_app_startable(&ctx) ? 1 : 0; }
SIM_API void sim_set_app_safe(int safe) { app_safe = (safe != 0); }
SIM_API void sim_fail_program_after(int n) { fail_program_after = n; }
SIM_API uint32_t sim_app_start(void) { return SIM_APP_START; }
SIM_API uint32_t sim_app_size(void) { return SIM_APP_SIZE; }

SIM_API void sim_read(uint32_t addr, uint8_t *out, uint32_t len)
{
    memcpy(out, &sim_flash[addr - SIM_META], len);
}
