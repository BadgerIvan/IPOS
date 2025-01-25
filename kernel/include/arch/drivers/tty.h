#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <arch/drivers/vga.h>

void terminal_initialize();
void terminal_clear(uint8_t color);
void terminal_putchar(int8_t c);
void terminal_write(FILE *stream, const char* data, uint32_t size);
void terminal_setcolor(uint8_t color);
void terminal_set_cursor(uint32_t pos);
uint32_t terminal_get_cursor();

#endif
