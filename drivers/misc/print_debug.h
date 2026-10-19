#ifndef INC_PRINT_DEBUG_H
#define INC_PRINT_DEBUG_H

#include <stdio.h>

/**
 * @brief Zero-overhead preprocessor logging framework.
 *
 * When DEBUG_PRINT_ENABLED is defined as non-zero, logs are routed to printf.
 * When disabled (default), all macros compile to ((void)0), generating zero code
 * and eliminating string literals and function call overhead from Flash/RAM.
 */
#if defined(DEBUG_PRINT_ENABLED) && (DEBUG_PRINT_ENABLED != 0)
    #define LOG_DEBUG(fmt, ...) printf("[DBG] " fmt "\r\n", ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)  printf("[INF] " fmt "\r\n", ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  printf("[WRN] " fmt "\r\n", ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) printf("[ERR] " fmt "\r\n", ##__VA_ARGS__)
    #define print_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...) ((void)0)
    #define LOG_INFO(fmt, ...)  ((void)0)
    #define LOG_WARN(fmt, ...)  ((void)0)
    #define LOG_ERROR(fmt, ...) ((void)0)
    #define print_debug(fmt, ...) ((void)0)
#endif

#endif /* INC_PRINT_DEBUG_H */
