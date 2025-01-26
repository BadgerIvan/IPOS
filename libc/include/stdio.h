#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

struct _FILE_s{
    int id;
    void *buffer;
    size_t size_buffer;
    size_t position;  
    int (*read)(struct FILE_s *stream, void *buf, size_t size); 
    int (*write)(struct FILE_s *stream, const void *buf, size_t size); 
    void (*flush)(struct FILE_s *stream); 
};
typedef struct _FILE_s FILE;

static FILE *stdout;

int vfprintf(FILE *stream, const char *format, va_list args);

int fprintf(FILE *stream, const char *format, ...);
int fputc(int ch, FILE *stream);
int fputs(const char *string, FILE *stream);

int printf(const char *format, ...);
int putc(int ch);
int puts(const char* string);

#ifdef __cplusplus
}
#endif

#endif
