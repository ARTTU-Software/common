#include "generic_ring_buffer.h"
#include <string.h>

static inline bool is_power_of_two(uint32_t x)
{
    return (x > 0U) && ((x & (x - 1U)) == 0U);
}

bool ring_buffer_init(Ring_Buffer_t *rb, uint8_t *storage, uint32_t size)
{
    if (rb == NULL || storage == NULL || !is_power_of_two(size)) {
        return false;
    }
    rb->buffer = storage;
    rb->size   = size;
    rb->head   = 0U;
    rb->tail   = 0U;
    return true;
}

bool ring_buffer_put(Ring_Buffer_t *rb, uint8_t byte)
{
    if (rb == NULL || rb->buffer == NULL) {
        return false;
    }
    if ((rb->head - rb->tail) >= rb->size) {
        return false; // Buffer full
    }
    rb->buffer[rb->head & (rb->size - 1U)] = byte;
    rb->head++;
    return true;
}

bool ring_buffer_get(Ring_Buffer_t *rb, uint8_t *byte)
{
    if (rb == NULL || rb->buffer == NULL || byte == NULL) {
        return false;
    }
    if (rb->head == rb->tail) {
        return false; // Buffer empty
    }
    *byte = rb->buffer[rb->tail & (rb->size - 1U)];
    rb->tail++;
    return true;
}

bool ring_buffer_peek(const Ring_Buffer_t *rb, uint8_t *byte)
{
    if (rb == NULL || rb->buffer == NULL || byte == NULL) {
        return false;
    }
    if (rb->head == rb->tail) {
        return false; // Buffer empty
    }
    *byte = rb->buffer[rb->tail & (rb->size - 1U)];
    return true;
}

size_t ring_buffer_write(Ring_Buffer_t *rb, const uint8_t *src, size_t len)
{
    if (rb == NULL || rb->buffer == NULL || src == NULL || len == 0U) {
        return 0U;
    }
    size_t written = 0U;
    while (written < len) {
        if (!ring_buffer_put(rb, src[written])) {
            break;
        }
        written++;
    }
    return written;
}

size_t ring_buffer_read(Ring_Buffer_t *rb, uint8_t *dst, size_t len)
{
    if (rb == NULL || rb->buffer == NULL || dst == NULL || len == 0U) {
        return 0U;
    }
    size_t read_bytes = 0U;
    while (read_bytes < len) {
        if (!ring_buffer_get(rb, &dst[read_bytes])) {
            break;
        }
        read_bytes++;
    }
    return read_bytes;
}

uint32_t ring_buffer_count(const Ring_Buffer_t *rb)
{
    if (rb == NULL) {
        return 0U;
    }
    return (rb->head - rb->tail);
}

uint32_t ring_buffer_free_space(const Ring_Buffer_t *rb)
{
    if (rb == NULL) {
        return 0U;
    }
    uint32_t used = rb->head - rb->tail;
    return (used >= rb->size) ? 0U : (rb->size - used);
}

bool ring_buffer_is_empty(const Ring_Buffer_t *rb)
{
    if (rb == NULL) {
        return true;
    }
    return (rb->head == rb->tail);
}

bool ring_buffer_is_full(const Ring_Buffer_t *rb)
{
    if (rb == NULL) {
        return false;
    }
    return ((rb->head - rb->tail) >= rb->size);
}

void ring_buffer_reset(Ring_Buffer_t *rb)
{
    if (rb != NULL) {
        rb->head = 0U;
        rb->tail = 0U;
    }
}
