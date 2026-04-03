#ifndef INC_GENERIC_RING_BUFFER_H
#define INC_GENERIC_RING_BUFFER_H

#include <stdint.h>

#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE 32
#endif

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t data[RING_BUFFER_SIZE];
} Generic_Ring_Buffer_t;

#endif /* INC_GENERIC_RING_BUFFER_H */
