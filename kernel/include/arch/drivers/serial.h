#ifndef _SERIAL_H 
#define _SERIAL_H

#include <stdint.h>
#include <stdio.h>

#define SERIAL_COM1 0x3F8      

int init_serial(uint16_t port);

void serial_write(const FILE *stream, const char *buffer, size_t size);

char serial_read(FILE *stream, char *buffer, size_t size);

#endif