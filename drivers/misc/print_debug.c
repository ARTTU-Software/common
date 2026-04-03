#include "print_debug.h"

/**
 * @brief Prints a formatted debug message to the console.
 * 
 * This function works like printf, but the output is conditionally compiled
 * based on the DEBUG_PRINT_ENABLED macro. If the macro is not defined or is 0,
 * the function call is compiled out, saving code space and execution time.
 * 
 * @param fmt A pointer to a null-terminated string that is written to the stream.
 *            It can contain format specifiers that are replaced by the values
 *            specified in subsequent additional arguments.
 * @param ... Additional arguments to format.
 */
void print_debug(const char* fmt, ...){
    #if DEBUG_PRINT_ENABLED
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args); // test ceedling thing
    va_end(args);
    #endif
}