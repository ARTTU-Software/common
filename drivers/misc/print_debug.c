#include "print_debug.h"
#include <stdarg.h>

#undef print_debug
void print_debug(const char* fmt, ...)
{
#if defined(DEBUG_PRINT_ENABLED) && (DEBUG_PRINT_ENABLED != 0)
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#else
    (void)fmt;
#endif
}