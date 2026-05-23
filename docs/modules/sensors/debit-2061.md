---
title: Common - Debit 2061
description: Pulse-based flow sensor driver.
---

# Debit 2061 Flow Sensor

Pulse-based flow sensor driver with EXTI increment and periodic rate computation.

## Functions
#### `void flow_sensor_init(FlowSensor_t* s, uint32_t pulses_per_litre, uint8_t port)`
Initializes the pulse counter and scaling.

#### `void debit_sensor_update(FlowSensor_t* s, uint32_t current_tick)`
Computes flow in ml/s based on pulse count and elapsed time.

#### `void flow_sensor_EXTI_handler(FlowSensor_t* s)`
ISR callback that increments the pulse counter.

#### `uint16_t debit_sensor_decode(uint8_t high_byte, uint8_t low_byte)`
Combines two bytes into a raw value.

### Usage
```c
#include "debit_2061.h"

static FlowSensor_t flow = {0};

void flow_init(void) {
    flow_sensor_init(&flow, 450U, 0);
}

void flow_exti_isr(void) {
    flow_sensor_EXTI_handler(&flow);
}

void flow_task(uint32_t now) {
    debit_sensor_update(&flow, now);
}
```

> [!WARNING]
> `flow_sensor_EXTI_handler` is intended for ISR context; keep it minimal.
