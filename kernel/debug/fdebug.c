#include <stdio.h>
#include <stdarg.h>

void debugf(const char *format, ...) {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
#endif
}