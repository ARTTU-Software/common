#ifndef INC_CAN_STM32_FDCAN_H
#define INC_CAN_STM32_FDCAN_H

#include "can_driver_core.h"

/**
 * @brief Optional STM32 FDCAN submit callback; context is an FDCAN handle pointer.
 * Compile can_stm32_fdcan.c with CAN_DRIVER_STM32_FDCAN_HEADER set to the board's
 * HAL header name. The portable core never includes that header. The board owns
 * clocks, pins, bit timing, message RAM, acceptance filters, start and recovery.
 * @return OK on hardware acceptance, BUSY when full, INVALID or IO_ERROR.
 */
can_driver_status_t can_stm32_fdcan_submit(void *context,
    const can_driver_frame_t *frame);

/**
 * @brief Drain at most budget FIFO entries from a board-owned RX callback.
 * Call only for the matching peripheral, from the sole RX producer context.
 * context is an FDCAN handle; fifo is the board HAL's FDCAN_RX_FIFO0/1 constant.
 * now_ms timestamps this bounded batch in milliseconds. Remote and FD frames
 * are rejected. On software overflow, drop newest and continue within budget.
 * A maximum-sized scratch buffer protects the copy before the DLC is checked.
 * The board must arrange another service opportunity if hardware entries remain.
 * @return OK, BUSY on software overflow, INVALID on rejected data/arguments,
 * NOT_INITIALIZED or IO_ERROR. The hardware FIFO is not polled in a wait loop.
 */
can_driver_status_t can_stm32_fdcan_receive_isr(can_driver_t *driver,
    void *context, uint32_t fifo, uint32_t now_ms, uint16_t budget);

#endif /* INC_CAN_STM32_FDCAN_H */
