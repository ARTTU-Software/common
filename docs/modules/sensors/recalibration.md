---
title: Common - Sensor Recalibration
description: Two-point sensor calibration with optional NVM storage.
---

# Sensor Recalibration

Two-point calibration helper with optional NVM persistence and CRC validation.

## Functions
#### `Recal_Instance_t* Recal_Init(Recal_Instance_t* inst, Recal_Callbacks_t const* cb, Recal_Config_t const* config)`
Initializes an instance and callbacks.

#### `int Recal_Start(Recal_Instance_t* inst)`
Starts a new calibration sequence.

#### `int Recal_Capture(Recal_Instance_t* inst)`
Captures the max point after the min window is sampled.

#### `int Recal_Abort(Recal_Instance_t* inst)`
Aborts and returns to idle.

#### `void Recal_Tick(Recal_Instance_t* inst, uint32_t dt_ms)`
Advances the calibration state machine. The current implementation does not use dt_ms.

#### `Recal_State_t Recal_GetState(Recal_Instance_t* inst)`
Returns the current state.

#### `const Recal_Data_t* Recal_GetData(Recal_Instance_t* inst)`
Returns the latest computed calibration data.

#### `int Recal_SaveToNvm(Recal_Instance_t* inst)`
Writes calibration data with CRC to NVM using callbacks.

#### `int Recal_LoadFromNvm(Recal_Instance_t* inst)`
Reads calibration data and validates CRC.

### Usage
```c
#include "generic_sensor_recal.h"

static uint32_t read_raw(void* ctx) {
    (void)ctx;
    return 1234U;
}

static int nvm_write(const void* data, size_t len, void* ctx) {
    (void)data; (void)len; (void)ctx;
    return 0;
}

static int nvm_read(void* data, size_t len, void* ctx) {
    (void)data; (void)len; (void)ctx;
    return 0;
}

void recal_example(void) {
    Recal_Instance_t inst;
    Recal_Callbacks_t cb = {
        .read_raw = read_raw,
        .nvm_write = nvm_write,
        .nvm_read = nvm_read,
        .context = 0,
    };
    Recal_Config_t cfg = { .sample_window = 16, .min_offset = 0, .max_offset = 0 };

    (void)Recal_Init(&inst, &cb, &cfg);
    (void)Recal_Start(&inst);
    Recal_Tick(&inst, 1U);
}
```
