---
title: Common - Debouncer
description: Threshold-based digital debouncer.
---

# Debouncer

Threshold-based digital debouncer for sampled inputs.

## Functions
#### `void debouncer_init(Debouncer_t* d, uint16_t threshold, uint32_t debounce_ms, uint8_t port)`
Initializes threshold and debounce timing.

#### `uint8_t digital_debouncer(Debouncer_t* d, uint16_t raw, uint32_t now)`
Returns the debounced state based on threshold and elapsed time.

### Usage
```c
#include "debouncer.h"

void debouncer_example(void) {
    Debouncer_t d = {0};
    debouncer_init(&d, 2000, 20, 0);
    (void)digital_debouncer(&d, 2500, 100);
}
```
