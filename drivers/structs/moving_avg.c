#include "moving_avg.h"
#include <string.h>

bool moving_avg_init(Moving_Avg_t *filter, uint32_t *storage, uint32_t window_size)
{
    if (filter == NULL || storage == NULL || window_size == 0U) {
        return false;
    }
    memset(storage, 0, window_size * sizeof(uint32_t));
    filter->buffer      = storage;
    filter->window_size = window_size;
    filter->count       = 0U;
    filter->index       = 0U;
    filter->accumulator = 0ULL;
    return true;
}

uint32_t moving_avg_update(Moving_Avg_t *filter, uint32_t sample)
{
    if (filter == NULL || filter->buffer == NULL || filter->window_size == 0U) {
        return 0U;
    }

    if (filter->count < filter->window_size) {
        // Window still filling up
        filter->buffer[filter->index] = sample;
        filter->accumulator += (uint64_t)sample;
        filter->count++;
        filter->index = (filter->index + 1U) % filter->window_size;
        return (uint32_t)(filter->accumulator / (uint64_t)filter->count);
    }

    // Window full: subtract outgoing oldest sample, add new sample
    filter->accumulator -= (uint64_t)filter->buffer[filter->index];
    filter->buffer[filter->index] = sample;
    filter->accumulator += (uint64_t)sample;
    filter->index = (filter->index + 1U) % filter->window_size;

    return (uint32_t)(filter->accumulator / (uint64_t)filter->window_size);
}

uint32_t moving_avg_get(const Moving_Avg_t *filter)
{
    if (filter == NULL || filter->count == 0U) {
        return 0U;
    }
    return (uint32_t)(filter->accumulator / (uint64_t)filter->count);
}

bool moving_avg_is_full(const Moving_Avg_t *filter)
{
    if (filter == NULL) {
        return false;
    }
    return (filter->count >= filter->window_size);
}

void moving_avg_reset(Moving_Avg_t *filter)
{
    if (filter != NULL && filter->buffer != NULL && filter->window_size > 0U) {
        memset(filter->buffer, 0, filter->window_size * sizeof(uint32_t));
        filter->count       = 0U;
        filter->index       = 0U;
        filter->accumulator = 0ULL;
    }
}
