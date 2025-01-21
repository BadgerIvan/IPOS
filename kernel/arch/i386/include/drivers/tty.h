#ifndef _TTY_H
#define _TTY_H 1

#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);
void terminal_putchar(int8_t c);
void terminal_write(const char* data, uint32_t size);
void terminal_setcolor(uint8_t color);
void terminal_set_cursor(uint32_t pos);
uint32_t terminal_get_cursor();

#endif
