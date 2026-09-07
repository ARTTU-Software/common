---
title: Common - Data Structures
description: Reusable, zero-allocation data structures and filter containers for embedded firmware modules.
---

# Data Structures <Badge type="info" text="SPSC & Filtering" />

We provide modular, zero-allocation data structures under `drivers/structs/` designed for real-time embedded safety and deterministic execution.

---

## 1. Single-Producer Single-Consumer Ring Buffer (`generic_ring_buffer.h`)

`Ring_Buffer_t` is a lock-free circular byte buffer designed for cross-context data passing (e.g. UART DMA / ISR producer to background thread consumer) without mutex locks or disabling interrupts.

### Data Structure

```c
typedef struct {
    uint8_t *buffer;           // Pointer to caller-allocated storage array
    uint32_t size;             // Buffer capacity in bytes (must be power of 2)
    volatile uint32_t head;    // Free-running write index (producer)
    volatile uint32_t tail;    // Free-running read index (consumer)
} Ring_Buffer_t;
```

### Key Engineering Features

* **Power-of-Two Masking**: Capacity is restricted to powers of two ($16, 32, 64, \dots$). Index wrap-around uses single-cycle bitwise masking (`head & (size - 1)`) instead of multi-cycle division.
* **100% Slot Utilization**: Uses monotonic free-running 32-bit indices. Unlike traditional modulo buffers that waste one slot to distinguish full from empty, this design holds the full $N$ bytes.
* **Zero Allocation**: The caller allocates the storage buffer statically in SRAM.

### API Reference

| Function | Signature | Description |
| :--- | :--- | :--- |
| `ring_buffer_init` | `(rb, storage, size)` | Validates power-of-two size and binds memory array |
| `ring_buffer_put` | `(rb, byte)` | Enqueues single byte. Returns `false` if full |
| `ring_buffer_get` | `(rb, byte_ptr)` | Dequeues single byte into pointer. Returns `false` if empty |
| `ring_buffer_peek` | `(rb, byte_ptr)` | Reads next byte without incrementing read index |
| `ring_buffer_write`| `(rb, src, len)` | Enqueues buffer stream. Returns count written |
| `ring_buffer_read` | `(rb, dst, len)` | Drains buffer stream. Returns count read |
| `ring_buffer_count`| `(rb)` | Returns number of bytes currently stored |
| `ring_buffer_free_space` | `(rb)` | Returns remaining free bytes |
| `ring_buffer_is_empty` / `is_full` | `(rb)` | State queries |
| `ring_buffer_reset`| `(rb)` | Resets head and tail to 0 |

### Usage Example

```c
#include "generic_ring_buffer.h"

static uint8_t uart_storage[64];
static Ring_Buffer_t uart_rx_rb;

void uart_setup(void) {
    ring_buffer_init(&uart_rx_rb, uart_storage, sizeof(uart_storage));
}

// ISR Context:
void UART_RX_IRQHandler(void) {
    uint8_t incoming_byte = (uint8_t)(USART1->RDR & 0xFF);
    ring_buffer_put(&uart_rx_rb, incoming_byte);
}

// Task Context:
void process_uart(void) {
    uint8_t byte;
    while (ring_buffer_get(&uart_rx_rb, &byte)) {
        handle_char(byte);
    }
}
```

---

## 2. Circular Moving Average Filter (`moving_avg.h`)

`Moving_Avg_t` implements a constant-time $\mathcal{O}(1)$ sliding boxcar filter for continuous analog sensors (e.g. suspension potentiometers, steering angle, brake pressure).

### Data Structure

```c
typedef struct {
    uint32_t *buffer;          // Caller-allocated window array
    uint32_t window_size;      // Capacity of window
    uint32_t count;            // Number of samples collected
    uint32_t index;            // Circular buffer write index
    uint64_t accumulator;      // Running 64-bit sum of active samples
} Moving_Avg_t;
```

### Key Engineering Features

* **$\mathcal{O}(1)$ Constant-Time Math**: As new samples arrive, the oldest sample is subtracted and the newest sample added to a 64-bit running sum:
  $$\text{accumulator} = \text{accumulator} - buffer[index] + new\_sample$$
  $$\text{average} = \frac{\text{accumulator}}{window\_size}$$
  Execution time remains identical regardless of whether the filter window is 4 samples or 256 samples.
* **Transient Ramp-Up**: While the window is filling ($count < window\_size$), the filter computes the exact mean of active samples, preventing step response lag on startup.

### API Reference

| Function | Signature | Description |
| :--- | :--- | :--- |
| `moving_avg_init` | `(filter, storage, window_size)` | Clears buffer and sets window capacity |
| `moving_avg_update` | `(filter, sample)` | Ingests new sample and returns current rolling average |
| `moving_avg_get` | `(filter)` | Returns current moving average without adding a sample |
| `moving_avg_is_full` | `(filter)` | Returns `true` once window has filled |
| `moving_avg_reset` | `(filter)` | Clears accumulator and zeroes buffer |

### Usage Example

```c
#include "moving_avg.h"

#define FILTER_WINDOW 16
static uint32_t steer_storage[FILTER_WINDOW];
static Moving_Avg_t steer_filter;

void init_steering_filter(void) {
    moving_avg_init(&steer_filter, steer_storage, FILTER_WINDOW);
}

uint32_t read_filtered_steering(uint32_t raw_adc) {
    return moving_avg_update(&steer_filter, raw_adc);
}
```
