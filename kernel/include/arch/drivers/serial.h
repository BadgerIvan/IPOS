#ifndef _SERIAL_H 
#define _SERIAL_H

#include <stdint.h>

#define SERIAL_COM1 0x3F8      

int init_serial(uint16_t);

void serial_write_char(const char a, uint16_t port);

void serial_write_string(const char* str, uint16_t port);

char serial_read(uint16_t port);

#endif