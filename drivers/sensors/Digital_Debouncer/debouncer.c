#include "debouncer.h"

void debouncer_init(Debouncer_t *d, uint16_t threshold, uint32_t debounce_ms, uint8_t port){
    d->threshold = threshold;
    d->debounce_ms = debounce_ms;
    d->last_raw = 0;
    d->last_change = 0;
    d->state = 0;
    d->port = port;
}

uint8_t digital_debouncer(Debouncer_t *d, uint16_t raw, uint32_t now){
    // Convert raw input with threshold
    uint8_t digital = (raw >= d->threshold) ? 1 : 0;

    // If raw changed, update last change timestamp
    if (digital != d->last_raw) {
        d->last_raw = digital;
        d->last_change = now;
    }

    // If stable long enough, update debounced state
    if ((now - d->last_change) >= d->debounce_ms) {
        d->state = digital;
    }

    return d->state;
}