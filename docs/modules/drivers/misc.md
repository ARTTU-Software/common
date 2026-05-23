---
title: Common - Misc Helpers
description: Debug print and timer calculation helpers.
---

# Misc Helpers

## Debug Print (print_debug.h)
Debug-only printf wrapper compiled out when DEBUG_PRINT_ENABLED is not defined or set to 0.

### Functions
#### `void print_debug(const char* fmt, ...)`
Prints a formatted message when DEBUG_PRINT_ENABLED is enabled.

### Usage
```c
#include "print_debug.h"

void example(void) {
    print_debug("CAN ready: %u\n", 1U);
}
```

## Timer Calculation Utils (timer_calc_utils.h)
Helper to compute PSC, ARR, and CCR for a PWM timer setup on 16-bit timers.

### Functions
#### `bool timer_calc_pwm(uint32_t freq_hz, uint8_t duty_percent, uint32_t bus_freq_mhz, uint16_t* psc, uint16_t* arr, uint16_t* ccr)`
Returns true when it can compute values within 16-bit limits.

### Usage
```c
#include "timer_calc_utils.h"

void timer_setup(void) {
    uint16_t psc = 0, arr = 0, ccr = 0;
    if (timer_calc_pwm(1000U, 50U, 170U, &psc, &arr, &ccr)) {
        /* Apply psc, arr, ccr to the timer HAL setup. */
    }
}
```
