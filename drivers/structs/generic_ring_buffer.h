#ifndef INC_GENERIC_RING_BUFFER_H
#define INC_GENERIC_RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) circular byte ring buffer.
 *
 * Buffer capacity MUST be a power of 2 (e.g. 16, 32, 64, 128, 256, 1024).
 * Uses free-running indices with power-of-two bitwise masking for zero-division wrap-around.
 */
typedef struct {
    uint8_t *buffer;           /**< Pointer to caller-allocated storage array */
    uint32_t size;             /**< Buffer capacity in bytes (must be power of 2) */
    volatile uint32_t head;    /**< Free-running write index (updated by producer) */
    volatile uint32_t tail;    /**< Free-running read index (updated by consumer) */
} Ring_Buffer_t;

// Backward-compatible typedef alias
typedef Ring_Buffer_t Generic_Ring_Buffer_t;

/**
 * @brief Initializes a ring buffer instance.
 * @param rb Pointer to Ring_Buffer_t instance.
 * @param storage Pointer to caller-allocated byte array.
 * @param size Capacity of the buffer in bytes (must be power of 2).
 * @return true on success, false if pointers are NULL or size is not power of 2.
 */
bool ring_buffer_init(Ring_Buffer_t *rb, uint8_t *storage, uint32_t size);

/**
 * @brief Pushes a single byte into the ring buffer.
 * @return true if byte was enqueued, false if buffer was full.
 */
bool ring_buffer_put(Ring_Buffer_t *rb, uint8_t byte);

/**
 * @brief Pops a single byte from the ring buffer.
 * @return true if byte was retrieved, false if buffer was empty.
 */
bool ring_buffer_get(Ring_Buffer_t *rb, uint8_t *byte);

/**
 * @brief Peeks at the next available byte without removing it.
 * @return true if byte was read, false if buffer was empty.
 */
bool ring_buffer_peek(const Ring_Buffer_t *rb, uint8_t *byte);

/**
 * @brief Writes multiple bytes into the ring buffer.
 * @return Number of bytes successfully written (may be less than len if full).
 */
size_t ring_buffer_write(Ring_Buffer_t *rb, const uint8_t *src, size_t len);

/**
 * @brief Reads multiple bytes from the ring buffer.
 * @return Number of bytes successfully read.
 */
size_t ring_buffer_read(Ring_Buffer_t *rb, uint8_t *dst, size_t len);

/**
 * @brief Returns the number of bytes currently stored in the buffer.
 */
uint32_t ring_buffer_count(const Ring_Buffer_t *rb);

/**
 * @brief Returns the remaining free space in bytes.
 */
uint32_t ring_buffer_free_space(const Ring_Buffer_t *rb);

/**
 * @brief Checks if the ring buffer is empty.
 */
bool ring_buffer_is_empty(const Ring_Buffer_t *rb);

/**
 * @brief Checks if the ring buffer is full.
 */
bool ring_buffer_is_full(const Ring_Buffer_t *rb);

/**
 * @brief Discards all buffer contents by resetting head and tail indices.
 */
void ring_buffer_reset(Ring_Buffer_t *rb);

#endif /* INC_GENERIC_RING_BUFFER_H */
