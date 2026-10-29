---
title: Portable CAN driver
description: Periodic commands and immediate status frames without HAL types in the common core.
---

# Portable CAN driver

A board often sends the same command repeatedly and sends a status frame when
something changes. The common driver provides a separate function for each job.
The board supplies a hardware handoff, a table of periodic messages and optional
receive storage. It does not need to build vendor headers or manage ring indices
in application code.

`can_driver_core.h` and `can_driver_core.c` have no HAL dependency. The optional
STM32 FDCAN adapter translates frames for the selected board HAL. An unrelated
CAN controller can implement the same submit callback.

## Two ways to send

| Function | Behavior |
| --- | --- |
| `can_driver_send_periodic(bus, now_ms)` | Attempt due table entries within a configured budget. A refused entry remains due. |
| `can_driver_send_now(bus, frame)` | Attempt hardware submission during this call. Return BUSY immediately when full. |

An immediate send has no hidden software queue. The caller decides whether an
event must be retained and retried. Two calls with the same ID remain two
separate attempts. Success means hardware accepted the frame; it does not prove
another node received it. Frames already accepted by hardware retain the
controller's ordering and arbitration behavior.

Periodic messages retain only the latest payload. After a task stall, one
transmission replaces the missed periods instead of replaying old commands.
Successful submission preserves the original phase. Failed submission leaves
the entry due, and the next call starts at the following slot so other messages
can progress. The first send is due one period after initialization.

The BMS provides useful examples of these policies: charger and actuator demands
need current payloads, while status publication retains a pending change until
hardware accepts it. Authorization must still be checked at the final handoff.
The driver cannot decide whether the car is permitted to enable an actuator.

## Bytes and frequency helpers

Both CAN driver headers include `can_helpers.h`. It can also be included alone
without HAL or driver state. Typed inline functions extract bytes by bit position,
independently of the CPU's memory byte order:

| Input width | Low byte | High byte |
| --- | --- | --- |
| 8 bits | `can_u8_low_byte(value)`: the value itself | `can_u8_high_byte(value)`: zero, because no second byte exists |
| 16 bits | `can_u16_low_byte(value)`: bits 7..0 | `can_u16_high_byte(value)`: bits 15..8 |
| 32 bits | `can_u32_low_byte(value)`: bits 7..0 | `can_u32_high_byte(value)`: bits 31..24 |

`can_u32_byte(value, index)` accesses all four bytes, with index zero selecting
the least significant byte. Indices outside 0..3 return zero. For an 8-bit field
split into two 4-bit values, use `can_u8_high_nibble` and `can_u8_low_nibble`.
`CAN_COMBINE_16` and `CAN_COMBINE_32` reconstruct values from bytes in
most-significant-first order. The legacy `HIGH_BYTE` macro keeps its original
bits 15..8 meaning, including when the input is a 32-bit value.

```c
uint32_t value = 0x12345678U;
payload[0] = can_u32_high_byte(value); /* 0x12 */
payload[1] = can_u32_byte(value, 2U);  /* 0x34 */
payload[2] = can_u32_byte(value, 1U);  /* 0x56 */
payload[3] = can_u32_low_byte(value);  /* 0x78 */
```

`HZ(50U)`, `HZ_TO_MS(50U)` and `CAN_HZ_TO_MS(50U)` all produce a constant 20 ms
interval for configuration tables. Inputs are integer frequencies in 1..1000 Hz.
Fractional milliseconds are truncated, so 60 Hz produces 16 ms. Zero and rates
above 1000 Hz return zero, which driver initialization rejects as an interval.
Use constants or side-effect-free expressions in these macros. The runtime
function `can_hz_to_ms(rate)` evaluates its argument once and uses the same
bounds and rounding.

## Configure one instance

This example uses project-defined message IDs. Their payload contracts and the
bus timing remain the board's responsibility.

```c
#include "can_driver_core.h"
#include "can_stm32_fdcan.h"
#include "fdcan.h"

static can_driver_t vehicle_can;
static can_driver_rx_frame_t rx_storage[17]; /* 16 usable entries. */
static can_driver_periodic_t periodic[] = {
    {
        .frame = {.id = APP_DEMAND_ID, .length = 8U},
        .period_ms = HZ(10U)
    }
};

static const can_driver_config_t config = {
    .submit = can_stm32_fdcan_submit,
    .context = &hfdcan1,
    .periodic = periodic,
    .periodic_count = sizeof(periodic) / sizeof(periodic[0]),
    .tx_budget = 1U,
    .rx_storage = rx_storage,
    .rx_capacity = sizeof(rx_storage) / sizeof(rx_storage[0])
};
```

Call `can_driver_init(&vehicle_can, &config, now_ms)` and check its result before
enabling RX notifications. CubeMX and the board binding continue to own clocks,
pins, bit timing, message RAM, acceptance filters and peripheral start. Configure
Classic CAN, sufficient RX/TX storage, and hardware filters for the subscribed IDs.
Set unmatched/remote acceptance explicitly. Bus-off recovery remains a board and
application policy.

Update a periodic payload with `can_driver_update_periodic(bus, slot, frame)` in
the owning task. The ID and standard/extended format must match the configured
slot. There is no event sentinel or zero-period message to configure. A project
which only sends events can leave the periodic table and count at zero.

In each service pass, check immediate work first if the application gives it
priority, then call `can_driver_send_periodic`. Both return BUSY without waiting.
Use the returned status to retain pending events or report a hardware failure.
This software order cannot recall a frame already submitted to hardware.

```c
can_driver_status_t status = can_driver_send_now(&vehicle_can, &status_frame);
if (status == CAN_DRIVER_OK) {
    status_pending = false;
} else if (status != CAN_DRIVER_BUSY) {
    app_can_record_fault(status);
}

status = can_driver_send_periodic(&vehicle_can, now_ms);
if (status != CAN_DRIVER_OK && status != CAN_DRIVER_BUSY) {
    app_can_record_fault(status);
}
```

Only call the first block when a status frame is pending. The driver has no
application heartbeat or status rate limiter; the BMS implements those separately.

## STM32 adapter build

Link the focused `common_can` target and compile the adapter with the board's HAL
includes. Select its HAL header explicitly:

```cmake
add_subdirectory(${COMMON_ROOT} common EXCLUDE_FROM_ALL)
target_link_libraries(firmware PRIVATE common_can)
target_sources(firmware PRIVATE
    ${COMMON_ROOT}/drivers/can/ports/can_stm32_fdcan.c)
target_compile_definitions(firmware PRIVATE
    CAN_DRIVER_STM32_FDCAN_HEADER="stm32h7xx_hal.h")
```

Use the board's G4, H5 or H7 HAL header as appropriate. The adapter uses vendor
DLC constants for lengths 0 through 8, including HAL releases with shifted
encodings. It constructs each TX header locally and copies the payload before
calling HAL, so no queued message borrows a temporary header pointer.

For a non-CMake project, add `can_driver_core.c`, the optional adapter source and
their two include directories directly, and enable C11.

The adapter compiles to no hardware implementation unless the header option is
defined. It does not install global HAL callbacks, start peripherals, or overwrite
CubeMX configuration. A guarded application transmit callback can inspect or
replace a command before delegating to `can_stm32_fdcan_submit`.

## Receiving and execution ownership

Call `can_stm32_fdcan_receive_isr` from the board's matching RX callback with a
FIFO, timestamp and nonzero batch budget. It rejects remote and FD frames and
converts the DLC before forwarding an inline frame copy to the core. Its 64-byte
scratch buffer accommodates HAL's copy before the received header can be checked.
The core supports Classic CAN payloads of 0 through 8 bytes.

After a bounded batch, arrange another interrupt/service opportunity if the
hardware FIFO still contains frames. For example, the board callback can check
`HAL_FDCAN_GetRxFifoFillLevel` and use `NVIC_SetPendingIRQ` with that board's RX
IRQ. Waiting only for another new-message event can leave the final queued
frames stranded. Record errors through an ISR-safe flag or counter; defer
application processing to the task.

The common core has exactly one RX producer and one task consumer. Its lock-free
atomic indices publish the completed copy before the consumer sees it. The
task uses `can_driver_receive` to obtain a copied frame and original receive
timestamp. A full software ring drops the newest frame, returns BUSY and
increments `rx_dropped`. It never overwrites the oldest unread input.

All TX functions, periodic updates, receive-pop calls and statistics reads belong
to one serialized task/main owner. Multiple tasks must serialize through that
owner. Do not call TX from an ISR or reinitialize storage while RX is active.
Each instance has its own scheduling, submission state and diagnostics. Periodic
and RX arrays must remain alive and must not be shared between active instances.
Treat their contents as driver-owned after initialization.

## Validation and diagnostics

Initialization rejects missing storage, zero/oversized periods, invalid IDs,
oversized payloads and duplicate periodic identities. A standard and an extended
frame with the same numeric ID are different identities. Periods are in
1 through INT32_MAX milliseconds. Pass a wrapping uint32_t millisecond clock and
service the driver at least once per timer wrap.

`can_driver_get_stats` copies per-instance hardware submission, BUSY, error, RX,
drop and invalid-ingress counters. RX counters are individually atomic; they are
not a simultaneous snapshot of every interrupt event. Adapter-rejected remote/FD
frames are reported by its return value before reaching those core counters.

The RX implementation requires C11 atomics that are always lock-free for unsigned
integers. A compile-time check rejects an implementation that might lock inside
an ISR. The core allocates no dynamic memory and never polls hardware in a wait.

## Verification and existing boards

Run `bundle exec ceedling test:all` from common. The host tests include the
portable scenarios, legacy compatibility and a HAL fixture with shifted DLCs.
The `test/renode/run_can_tests.py` runner cross-compiles the same
portable scenarios for Cortex-M4, M7 and M33 and runs Robot assertions on each.
See `test/renode/README.md` in the repository for the command and scope.

`generic_can_driver.h` remains available for existing projects, including current
BMS callers. Its `CAN_send_single_frame` still queues a frame; it has not acquired
the new immediate-send semantics. Migrate explicitly rather than renaming the
old function or removing the BMS's last-moment authorization and cancellation.
The [legacy guide](can-integration) describes that older API.
