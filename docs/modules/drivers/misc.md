---
title: Common - Misc Helpers
description: Zero-cost preprocessor logging and timer register calculation utilities.
---

# Misc Helpers <Badge type="info" text="Utilities" />

We maintain shared utilities under `drivers/misc/` for debug logging and hardware timer calculations.

---

## 1. Zero-Cost Debug Logging (`print_debug.h`)

`print_debug.h` provides preprocessor logging macros with standard log levels. 

### Behavior & Macro Suppression

| Macro | Prefix | Enabled (`DEBUG_PRINT_ENABLED != 0`) | Disabled (Default) |
| :--- | :--- | :--- | :--- |
| `LOG_DEBUG(fmt, ...)` | `[DBG]` | `printf("[DBG] " fmt "\r\n", ...)` | `((void)0)` |
| `LOG_INFO(fmt, ...)`  | `[INF]` | `printf("[INF] " fmt "\r\n", ...)` | `((void)0)` |
| `LOG_WARN(fmt, ...)`  | `[WRN]` | `printf("[WRN] " fmt "\r\n", ...)` | `((void)0)` |
| `LOG_ERROR(fmt, ...)` | `[ERR]` | `printf("[ERR] " fmt "\r\n", ...)` | `((void)0)` |
| `print_debug(fmt, ...)`| None   | `printf(fmt, ...)`                  | `((void)0)` |

> [!TIP]
> **Zero Flash & Cycle Overhead**: When `DEBUG_PRINT_ENABLED` is `0` or undefined, all logging statements compile down to `((void)0)`. The compiler completely strips string literals, arguments, and function calls from the binary, eliminating CPU overhead and `.rodata` Flash consumption.

### Usage Example

```c
#include "print_debug.h"

void process_frame(uint32_t can_id, uint8_t len) {
    LOG_INFO("Processing frame ID 0x%03lX, DLC %u", can_id, len);
    if (len > 8) {
        LOG_WARN("Payload exceeded 8 bytes, truncating");
    }
}
```

---

## 2. Timer Calculation Utils (`timer_calc_utils.h`)

Calculates STM32 16-bit timer register values (Prescaler, Auto-Reload, and Capture/Compare) for periodic interrupts and PWM generation.

### Mathematical Formulation

The driver calculates required timer ticks directly from bus frequency and target frequency:

$$\text{total\_ticks} = \frac{\text{bus\_freq\_hz}}{\text{target\_freq\_hz}} = \frac{\text{bus\_freq\_mhz} \times 1\,000\,000}{\text{freq\_hz}}$$

$$\begin{cases}
PSC = 0, \quad ARR = \text{total\_ticks} - 1 & \text{if } \text{total\_ticks} \le 65536 \\
PSC = \lceil \frac{\text{total\_ticks}}{65536} \rceil - 1, \quad ARR = \lfloor \frac{\text{total\_ticks}}{PSC + 1} \rfloor - 1 & \text{otherwise}
\end{cases}$$

### API Reference

#### `bool timer_calculate_freq_psc_arr(freq_hz, bus_freq_mhz, psc, arr)`

Computes Prescaler and Auto-Reload values directly from frequency in Hertz. Supports high-frequency PWM and periodic timers (e.g. $10\text{ Hz}$ to $100\text{ kHz}$).

#### `bool timer_calculate_psc_arr(period_ms, bus_freq_mhz, psc, arr)`

Computes Prescaler and Auto-Reload values from period in milliseconds.

#### `bool timer_calc_pwm(freq_hz, duty_percent, bus_freq_mhz, psc, arr, ccr)`

Computes all 3 registers (PSC, ARR, and CCR) for a requested PWM frequency and duty cycle ($0\text{--}100\%$).

### Usage Example

```c
#include "timer_calc_utils.h"

void buzzer_pwm_setup(void) {
    uint16_t psc = 0, arr = 0, ccr = 0;

    // 10 kHz PWM, 50% duty on a 170 MHz bus
    if (timer_calc_pwm(10000U, 50U, 170U, &psc, &arr, &ccr)) {
        htim1.Instance->PSC  = psc;
        htim1.Instance->ARR  = arr;
        htim1.Instance->CCR1 = ccr;
    }
}
```
