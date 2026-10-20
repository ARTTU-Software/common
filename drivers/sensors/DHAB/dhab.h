#ifndef INC_DHAB_H
#define INC_DHAB_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Supported LEM transfer characteristics, at a nominal 5 V supply. */
typedef enum {
    DHAB_S118, /**< CH1: 30 A, 66.7 mV/A; CH2: 350 A, 5.7 mV/A. */
    DHAB_S134  /**< CH1: 50 A, 40 mV/A; CH2: 200 A, 10 mV/A. */
} dhab_model_t;

/** @brief Selected measurement. LOW and HIGH also index channel arrays. */
typedef enum {
    DHAB_LOW = 0,
    DHAB_HIGH = 1,
    DHAB_NONE = 2
} dhab_range_t;

/** @brief Reasons a channel must not be used as a current measurement. */
typedef enum {
    DHAB_SAMPLE_OK = 0,
    DHAB_SAMPLE_INPUT = 1u << 0, /**< Invalid reference, supply or ADC count. */
    DHAB_SAMPLE_ADC_RAIL = 1u << 1,
    DHAB_SAMPLE_CLAMP = 1u << 2, /**< Inside the possible output clamp bands. */
    DHAB_SAMPLE_RANGE = 1u << 3,
    DHAB_SAMPLE_SETTLING = 1u << 4,
    DHAB_SAMPLE_DISAGREEMENT = 1u << 5 /**< Usable channels differ by over 5 A. */
} dhab_sample_flag_t;

/** @brief Board-specific analog path; these values are not sensor constants. */
typedef struct {
    float divider_gain; /**< Sensor-side voltage / ADC-side voltage, >= 1. */
    float diode_drop_v; /**< Series drop before divider; use 0 without a diode. */
    float zero_offset_v; /**< Measured zero error, normalized to 5 V supply. */
} dhab_channel_config_t;

/** @brief Explicit raw-ADC contract. The caller owns calibration and sample timing. */
typedef struct {
    dhab_model_t model;
    uint32_t adc_full_scale; /**< Maximum output code AFTER oversampling shift. */
    dhab_channel_config_t channel[2];
} dhab_config_t;

/** @brief Unfiltered channel diagnostics, including invalid/out-of-range samples. */
typedef struct {
    uint32_t adc_raw;
    float adc_v;
    float sensor_v;
    float current_a;
    uint32_t flags; /**< Zero means usable. Always check before using current_a. */
} dhab_channel_sample_t;

/** @brief One paired acquisition; invalid selection has current_ma=0 and range=NONE. */
typedef struct {
    dhab_channel_sample_t channel[2];
    uint32_t timestamp_ms;
    int32_t current_ma; /**< Signed mA; int16_t at x100 cannot hold 350 A. */
    dhab_range_t range;
    bool valid;
} dhab_sample_t;

/** @brief Caller-owned state; one task must own init/update and sample access. */
typedef struct {
    dhab_config_t config;
    dhab_sample_t sample;
    uint32_t started_ms;
    uint32_t last_overload_ms[2];
    bool recovering[2];
    bool initialized;
} dhab_t;

/** @brief Initialize a sensor; clears state even on invalid configuration.
 * @param sensor Caller-owned instance, never shared with an ISR.
 * @param config Model, ADC scale and independent analog path for each channel.
 * @param now_ms Monotonic milliseconds, wrapping naturally at UINT32_MAX.
 * @return true on success; false for NULL or invalid configuration. */
bool dhab_init(dhab_t *sensor, const dhab_config_t *config, uint32_t now_ms);

/** @brief Convert an unfiltered pair and select the usable low/high range.
 * @param sensor Initialized, task-owned instance; diagnostics are replaced each call.
 * @param low_raw CH1 ADC code, before any generic sensor/Kalman conversion.
 * @param high_raw CH2 ADC code from the same scan.
 * @param supply_v Actual sensor supply in V (4.75 to 5.25).
 * @param adc_vref_v Actual ADC reference in V, positive and finite.
 * @param now_ms Acquisition time in monotonic milliseconds. Call at least every 10 ms.
 * @return true if a selected current is usable, false otherwise. Both channel
 * diagnostics remain available. Enforces 1 ms startup and 10 ms after overload.
 * Hysteresis switches to high above 90% of low range, back below 80%.
 * In the shared usable range, disagreement above 5 A invalidates both channels.
 * This conservative plausibility threshold is software policy, not an accuracy claim.
 * There is no automatic zero calibration, Kalman filter or CAN encoding. */
bool dhab_update(dhab_t *sensor, uint32_t low_raw, uint32_t high_raw,
                 float supply_v, float adc_vref_v, uint32_t now_ms);

#endif /* INC_DHAB_H */
