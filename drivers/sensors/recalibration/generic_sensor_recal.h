#ifndef INC_GENERIC_SENSOR_RECAL_H
#define INC_GENERIC_SENSOR_RECAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
// Generic sensor recalibration driver

// Public states for the recalibration FSM
typedef enum {
    RECAL_STATE_IDLE = 0,
    RECAL_STATE_MEASURE_MIN_SAMPLING,
    RECAL_STATE_WAIT_MAX_CAPTURE,
    RECAL_STATE_MEASURE_MAX_SAMPLING,
    RECAL_STATE_COMPUTE,
    RECAL_STATE_STORE,
    RECAL_STATE_VERIFY,
    RECAL_STATE_COMPLETED,
    RECAL_STATE_ERROR
} Recal_State_t;

// Error codes
#define RECAL_OK               0
#define RECAL_ERR_NOT_STARTED -1
#define RECAL_ERR_INVALID_ARG -2
#define RECAL_ERR_PLAUSIBLE   -3
#define RECAL_ERR_NVM         -4

// Stored calibration layout
typedef struct {
    uint16_t version;    // layout version
    float offset;        // offset to apply to raw
    float scale;         // scale factor
    uint32_t raw_min;    // raw min reference
    uint32_t raw_max;    // raw max reference
    uint32_t crc;        // CRC32 of preceding fields
} Recal_Data_t;

// Sensor-specific callbacks the generic driver uses
typedef struct {
    // Return latest raw sample (ADC counts or sensor raw units)
    uint32_t (*read_raw)(void *context);

    // Persistent storage hooks: write/read calibration blob
    // Should return 0 on success, negative on error
    int (*nvm_write)(const void *data, size_t len, void *context);
    int (*nvm_read)(void *data, size_t len, void *context);

    void *context; // user context passed to callbacks
} Recal_Callbacks_t;

// Runtime configuration for a calibration sequence.
// min_offset is added to the captured minimum, max_offset is subtracted from
// the captured maximum. This creates a small deadzone at both ends so the
// calibrated output does not flicker at 0% or 100% due to noise.
typedef struct {
    uint32_t sample_window;
    uint16_t min_offset;
    uint16_t max_offset;
} Recal_Config_t;

// Instance type (was opaque) - exposed so callers can allocate statically
typedef struct Recal_Instance {
    Recal_Callbacks_t cb;
    Recal_Data_t data;
    Recal_State_t state;

    uint32_t sample_window;
    uint16_t min_offset;
    uint16_t max_offset;

    uint64_t sample_acc;
    uint32_t sample_count;

    bool have_min;
    uint32_t measured_min;
    uint32_t measured_max;

    int last_error;
} Recal_Instance_t;

// Create/initialize an instance (caller provides memory)
Recal_Instance_t *Recal_Init(Recal_Instance_t *inst,
                             Recal_Callbacks_t const *cb,
                             Recal_Config_t const *config);

// Start a new 2-point linear calibration sequence
int Recal_Start(Recal_Instance_t *inst);

// Trigger a user 'capture' (first capture -> min, second -> max)
int Recal_Capture(Recal_Instance_t *inst);

// Abort current calibration
int Recal_Abort(Recal_Instance_t *inst);

// Periodic tick in milliseconds; call from main loop or systick
void Recal_Tick(Recal_Instance_t *inst, uint32_t dt_ms);

// Query current state
Recal_State_t Recal_GetState(Recal_Instance_t *inst);

// Get pointer to latest computed calibration data (valid after COMPUTE/STORE/VERIFY)
const Recal_Data_t *Recal_GetData(Recal_Instance_t *inst);

// Load/Save helpers that call user nvm callbacks directly
int Recal_SaveToNvm(Recal_Instance_t *inst);
int Recal_LoadFromNvm(Recal_Instance_t *inst);

#endif /* INC_GENERIC_SENSOR_RECAL_H */
