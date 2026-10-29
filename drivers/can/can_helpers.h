#ifndef INC_CAN_HELPERS_H
#define INC_CAN_HELPERS_H

#include <stdint.h>

/**
 * @brief Convert a constant integer frequency in Hz to whole milliseconds.
 * Valid frequencies are 1..1000 Hz. Fractional milliseconds are truncated;
 * zero or an out-of-range rate returns zero (an invalid periodic interval).
 * Use constants or side-effect-free arguments. For runtime expressions use
 * can_hz_to_ms(), which evaluates the caller's expression exactly once.
 */
#define CAN_HZ_TO_MS(hz) \
    ((((hz) > 0U) && ((hz) <= 1000U)) ? ((uint32_t)(1000U / (hz))) : 0U)

/** @brief Short configuration spelling: HZ(50U) is a 20 ms interval. */
#ifndef HZ
#define HZ(hz) (CAN_HZ_TO_MS(hz))
#endif

/** @brief Compatibility spelling for the frequency conversion macro. */
#ifndef HZ_TO_MS
#define HZ_TO_MS(hz) (CAN_HZ_TO_MS(hz))
#endif

/** @brief Extract bits 7..0; remains usable in constant initializers. */
#define LOW_BYTE(value) ((uint8_t)((uint32_t)(value) & 0xffU))

/** @brief Legacy extraction of bits 15..8, including for a 32-bit input. */
#define HIGH_BYTE(value) ((uint8_t)(((uint32_t)(value) >> 8U) & 0xffU))

/** @brief Combine two bytes in most-significant-byte-first order. */
#define CAN_COMBINE_16(high, low) \
    ((uint16_t)(((uint32_t)(uint8_t)(high) << 8U) | (uint32_t)(uint8_t)(low)))

/* Convert before shifting so a byte with bit 7 set cannot overflow signed int. */
/** @brief Combine four bytes in most-significant-byte-first order. */
#define CAN_COMBINE_32(b3, b2, b1, b0) \
    (((uint32_t)(uint8_t)(b3) << 24U) | ((uint32_t)(uint8_t)(b2) << 16U) | \
     ((uint32_t)(uint8_t)(b1) << 8U) | (uint32_t)(uint8_t)(b0))

/** @brief Return the only byte of an 8-bit value. */
static inline uint8_t can_u8_low_byte(uint8_t value)
{
    return value;
}

/** @brief Return zero: an 8-bit value has no second/high byte. */
static inline uint8_t can_u8_high_byte(uint8_t value)
{
    (void)value;
    return 0U;
}

/** @brief Return bits 3..0 as a value in 0..15. */
static inline uint8_t can_u8_low_nibble(uint8_t value)
{
    return (uint8_t)(value & 0x0fU);
}

/** @brief Return bits 7..4 as a value in 0..15. */
static inline uint8_t can_u8_high_nibble(uint8_t value)
{
    return (uint8_t)(value >> 4U);
}

/** @brief Return bits 7..0 of a 16-bit value. */
static inline uint8_t can_u16_low_byte(uint16_t value)
{
    return LOW_BYTE(value);
}

/** @brief Return bits 15..8 of a 16-bit value. */
static inline uint8_t can_u16_high_byte(uint16_t value)
{
    return HIGH_BYTE(value);
}

/** @brief Return bits 7..0 of a 32-bit value. */
static inline uint8_t can_u32_low_byte(uint32_t value)
{
    return LOW_BYTE(value);
}

/** @brief Return bits 31..24 of a 32-bit value. */
static inline uint8_t can_u32_high_byte(uint32_t value)
{
    return (uint8_t)(value >> 24U);
}

/**
 * @brief Return byte index 0..3, counted from the least significant byte.
 * Indices outside 0..3 return zero without performing an invalid shift.
 */
static inline uint8_t can_u32_byte(uint32_t value, uint8_t index)
{
    if (index > 3U) {
        return 0U;
    }
    return (uint8_t)(value >> ((uint32_t)index * 8U));
}

/** @brief Runtime Hz conversion with the same bounds/rounding as CAN_HZ_TO_MS. */
static inline uint32_t can_hz_to_ms(uint32_t hz)
{
    return CAN_HZ_TO_MS(hz);
}

#endif /* INC_CAN_HELPERS_H */
