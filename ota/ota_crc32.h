#ifndef INC_OTA_CRC32_H
#define INC_OTA_CRC32_H

#include <stdint.h>

/*
 * CRC-32 (IEEE 802.3, reflected, poly 0xEDB88320, init/xorout 0xFFFFFFFF).
 * Same result as Python's zlib.crc32().
 */

#define OTA_CRC32_INIT 0xFFFFFFFFU

/** Feed bytes into a running CRC. Start with OTA_CRC32_INIT. */
uint32_t ota_crc32_update(uint32_t crc, const uint8_t *buf, uint32_t len);

/** Finalize a running CRC. */
static inline uint32_t ota_crc32_final(uint32_t crc) { return crc ^ 0xFFFFFFFFU; }

/** One-shot CRC over a buffer. */
uint32_t ota_crc32(const uint8_t *buf, uint32_t len);

#endif /* INC_OTA_CRC32_H */
