#ifndef INC_MOVING_AVG_H
#define INC_MOVING_AVG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Circular moving average (boxcar) filter for analog sensor readings.
 */
typedef struct {
    uint32_t *buffer;          /**< Pointer to caller-allocated sample buffer */
    uint32_t window_size;      /**< Total sample capacity */
    uint32_t count;            /**< Number of samples currently collected */
    uint32_t index;            /**< Current circular buffer write pointer */
    uint64_t accumulator;      /**< Running sum of active samples */
} Moving_Avg_t;

/**
 * @brief Initializes a moving average filter instance.
 * @param filter Pointer to Moving_Avg_t instance.
 * @param storage Caller-allocated array of uint32_t elements.
 * @param window_size Number of samples in the averaging window (must be > 0).
 * @return true on success, false on invalid arguments.
 */
bool moving_avg_init(Moving_Avg_t *filter, uint32_t *storage, uint32_t window_size);

/**
 * @brief Feeds a new sample into the filter and returns the updated average.
 * @param filter Pointer to Moving_Avg_t instance.
 * @param sample New raw sample value.
 * @return Computed moving average.
 */
uint32_t moving_avg_update(Moving_Avg_t *filter, uint32_t sample);

/**
 * @brief Returns the latest computed average without pushing a new sample.
 * @return Current moving average, or 0 if empty or filter is NULL.
 */
uint32_t moving_avg_get(const Moving_Avg_t *filter);

/**
 * @brief Checks if the moving average window is completely filled.
 */
bool moving_avg_is_full(const Moving_Avg_t *filter);

/**
 * @brief Resets the filter accumulators and buffer contents.
 */
void moving_avg_reset(Moving_Avg_t *filter);

#endif /* INC_MOVING_AVG_H */
