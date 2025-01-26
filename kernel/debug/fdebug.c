#include <stdio.h>
#include <stdarg.h>

void debugf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}