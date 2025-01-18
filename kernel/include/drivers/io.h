#include <stdint.h>

#ifndef _IO_H
#define _IO_H 1

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);
unsigned short port_word_in (unsigned short port);
void port_word_out (unsigned short port, unsigned short data);

#endif