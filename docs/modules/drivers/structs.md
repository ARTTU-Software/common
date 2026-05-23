---
title: Common - Structs
description: Basic data structures used by the shared drivers.
---

# Structs

## Generic Ring Buffer (generic_ring_buffer.h)
Minimal byte ring buffer struct. No helper functions are provided; callers manage head and tail updates.

### Types
- Generic_Ring_Buffer_t: head, tail, and a fixed-size data array (default size 32).

### Usage
```c
#include "generic_ring_buffer.h"

void ring_buffer_example(void) {
    Generic_Ring_Buffer_t rb = {0};
    rb.data[rb.head++] = 0xA5;
}
```
