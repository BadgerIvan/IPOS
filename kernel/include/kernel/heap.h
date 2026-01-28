#ifndef _HEAP_H
#define _HEAP_H

#include <stddef.h>

void *malloc(size_t);
void *realloc(void *, size_t);
void *calloc(size_t, size_t);
void free(void *);

#endif