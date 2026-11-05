#include "ota_stm32_common.h"

#if defined(OTA_STM32_SUPPORTED)

#include "ota_protocol.h"

static FDCAN_HandleTypeDef *ota_hfdcan;
static ota_boot_t ota_ctx;
static ota_layout_t ota_layout;
static bool ota_flash_writable;   /* false: board layout does not fit this part, refuse updates */

/* ── port glue for ota_boot ───────────────────────────────────── */

static bool port_can_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    return ota_stm32_can_send(ota_hfdcan, id, data, len);
}

static bool port_can_recv(uint32_t *id, uint8_t *data, uint8_t *len)
{
    return ota_stm32_can_recv(ota_hfdcan, id, data, len);
}

static uint32_t port_tick_ms(void)
{
    return HAL_GetTick();
}

static bool port_flash_erase(uint32_t addr)
{
    return ota_flash_writable && ota_stm32_flash_erase(addr);
}

static bool port_flash_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    return ota_flash_writable && ota_stm32_flash_program(addr, data, len);
}

static const uint8_t *port_flash_ptr(uint32_t addr)
{
    return (const uint8_t *)addr;
}

static const ota_port_t ota_port = {
    .can_send      = port_can_send,
    .can_recv      = port_can_recv,
    .tick_ms       = port_tick_ms,
    .flash_erase   = port_flash_erase,
    .flash_program = port_flash_program,
    .flash_ptr     = port_flash_ptr,
};

/* ── CAN ──────────────────────────────────────────────────────── */

static void ota_can_start(uint8_t node_id)
{
    FDCAN_FilterTypeDef f = {0};

    if (ota_hfdcan->State != HAL_FDCAN_STATE_READY) {
        (void)HAL_FDCAN_Stop(ota_hfdcan);
    }

    if (ota_hfdcan->Init.StdFiltersNbr >= 2U) {
        /* Only our own OTA IDs reach the 3-deep RX FIFO; everything else is dropped in hardware. */
        f.IdType       = FDCAN_STANDARD_ID;
        f.FilterIndex  = 0U;
        f.FilterType   = FDCAN_FILTER_RANGE;
        f.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        f.FilterID1    = OTA_ID_CMD(node_id);
        f.FilterID2    = OTA_ID_DATA(node_id);
        (void)HAL_FDCAN_ConfigFilter(ota_hfdcan, &f);

        f.FilterIndex  = 1U;
        f.FilterType   = FDCAN_FILTER_DUAL;
        f.FilterID1    = OTA_ID_ENTER_REQUEST;
        f.FilterID2    = OTA_ID_ENTER_REQUEST;
        (void)HAL_FDCAN_ConfigFilter(ota_hfdcan, &f);

        (void)HAL_FDCAN_ConfigGlobalFilter(ota_hfdcan, FDCAN_REJECT, FDCAN_REJECT,
                                           FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
    } else {
        /* No filter elements configured in CubeMX: take everything, ota_boot ignores foreign IDs.
         * Works, but a busy bus can overrun the FIFO while flash is being erased. */
        (void)HAL_FDCAN_ConfigGlobalFilter(ota_hfdcan, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_REJECT,
                                           FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
    }
    (void)HAL_FDCAN_Start(ota_hfdcan);
}

/* ── layout ───────────────────────────────────────────────────── */

/*
 * Take erase/program units from the part and clamp the app area to the real
 * flash size. If the board's addresses do not match this part's erase
 * granularity (e.g. an L5 with 4 KB pages), keep booting the existing app but
 * refuse every flash write: START then fails with ERR_ERASE before anything
 * is touched, instead of erasing the wrong page.
 */
static void ota_layout_fixup(const ota_layout_t *board)
{
    uint32_t unit = ota_stm32_flash_erase_unit();
    uint32_t flash_end = ota_stm32_flash_end();

    ota_layout = *board;
    ota_layout.erase_unit = unit;
    ota_layout.program_unit = ota_stm32_flash_program_unit();

    if (ota_layout.app_end > flash_end) {
        ota_layout.app_end = flash_end;
    }
    ota_flash_writable = ((ota_layout.meta_addr % unit) == 0U) && ((ota_layout.app_start % unit) == 0U) &&
                         ((ota_layout.meta_addr + unit) <= ota_layout.app_start) &&
                         (ota_layout.app_end > ota_layout.app_start);
}

/* ── jump ─────────────────────────────────────────────────────── */

static void ota_jump_to_app(uint32_t app_start)
{
    uint32_t sp = *(volatile const uint32_t *)app_start;
    uint32_t entry = *(volatile const uint32_t *)(app_start + 4U);

    (void)HAL_FDCAN_Stop(ota_hfdcan);
    (void)HAL_FDCAN_DeInit(ota_hfdcan);
    (void)HAL_RCC_DeInit();
    (void)HAL_DeInit();

    __disable_irq();
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;
    for (uint32_t i = 0U; i < (sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0])); i++) {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }

    SCB->VTOR = app_start;
    __DSB();
    __ISB();
#if defined(__ARM_ARCH_8M_MAIN__) && (__ARM_ARCH_8M_MAIN__ == 1)
    __set_MSPLIM(0U); /* Cortex-M33 (H5/L5): drop the bootloader's stack limit */
#endif

    /* Nothing is enabled in the NVIC any more, so it is safe to leave PRIMASK cleared for the app. */
    __enable_irq();

    __asm volatile (
        "msr msp, %0 \n"
        "bx  %1      \n"
        :
        : "r" (sp), "r" (entry)
        : "memory");

    while (1) {
    }
}

/* ── entry ────────────────────────────────────────────────────── */

__weak void ota_bootloader_on_poll(uint8_t ota_state, uint32_t tick_ms)
{
    (void)ota_state;
    (void)tick_ms;
}

void ota_bootloader_run(FDCAN_HandleTypeDef *hfdcan, const ota_layout_t *layout)
{
    bool requested = ota_stm32_boot_flag_take();

    ota_hfdcan = hfdcan;
    ota_layout_fixup(layout);
    ota_can_start(ota_layout.node_id);
    ota_boot_init(&ota_ctx, &ota_layout, &ota_port);

    if (!requested && ota_boot_app_startable(&ota_ctx)) {
        /* Short window so a host can catch a node whose app crashes or ignores 0x4F0. */
        uint32_t t0 = HAL_GetTick();

        while (!ota_ctx.host_seen && ((HAL_GetTick() - t0) < OTA_BOOT_LISTEN_MS)) {
            (void)ota_boot_poll(&ota_ctx);
        }
        if (!ota_ctx.host_seen) {
            ota_jump_to_app(ota_layout.app_start);
        }
    }

    while (1) {
        ota_boot_action_t action = ota_boot_poll(&ota_ctx);

        ota_bootloader_on_poll(ota_ctx.state, HAL_GetTick());

        if (action == OTA_BOOT_ACTION_JUMP_TO_APP) {
            ota_jump_to_app(ota_layout.app_start);
        } else if (action == OTA_BOOT_ACTION_RESET) {
            ota_stm32_can_drain_tx(ota_hfdcan);
            NVIC_SystemReset();
        }
    }
}

#endif /* OTA_STM32_SUPPORTED */
