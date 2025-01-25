#include <stdio.h>
#include <stdarg.h>

int fprintf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = fvprintf(stream, format, args);
    va_end(args);
    return result;
}