#ifndef INC_DEBOUNCER_H
#define INC_DEBOUNCER_H

#include <stdint.h>

typedef struct {
    uint8_t state;           // debounced stable state
    uint8_t last_raw;        // previous raw value (0/1)
    uint32_t last_change;    // timestamp when raw last changed
    uint32_t debounce_ms;    // debounce threshold in ms/ticks
    uint16_t threshold;      // raw >= threshold => 1, else 0
    uint8_t port;
} Debouncer_t;

void debouncer_init(Debouncer_t *d, uint16_t threshold, uint32_t debounce_ms, uint8_t port);
uint8_t digital_debouncer(Debouncer_t *d, uint16_t raw, uint32_t now);

#endif /* INC_DEBOUNCER_H */
