#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/vga.h>
#include <arch/drivers/io.h>

#define VGA_COMMAND_PORT 0x3d4
#define VGA_DATA_PORT 0x3d5
#define VGA_HIGH_BYTE_COMMAND 0x0e
#define VGA_LOW_BYTE_COMMAND 0x0f

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t*)0xB8000

static uint32_t terminal_row;
static uint32_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static uint32_t get_offset(uint32_t column, uint32_t row) {
    return 2 * (row * VGA_WIDTH + column);
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	terminal_clear(terminal_color);
}

void terminal_clear(uint8_t color) {
	for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			terminal_putentryat(' ', color, x, y);
		}
	}
	terminal_column = 0;
	terminal_row = 0;
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_newline() {
	if(++terminal_row == VGA_HEIGHT) {
		memmove(VGA_MEMORY, VGA_MEMORY + VGA_WIDTH, VGA_HEIGHT * (VGA_WIDTH - 1) * sizeof(uint16_t));
		uint16_t entry = vga_entry(' ', terminal_color);;
		for(uint32_t *i = VGA_MEMORY + (VGA_HEIGHT * (VGA_WIDTH - 1)); i < VGA_MEMORY + (VGA_HEIGHT * VGA_WIDTH); i++) {
			*(i) = entry;
		}
		terminal_row = VGA_HEIGHT - 1;
	}
	terminal_column = 0;
}

void terminal_putentryat(uint8_t c, uint8_t color, uint32_t x, uint32_t y) {
	const uint32_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(int8_t ch) {
	uint8_t uc = (uint8_t)ch;

	switch(uc) 
	{
	case '\r':
		terminal_column = 0;
		break;
	case '\n':
		terminal_newline();
		break;
	default:
		terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
		terminal_newline();
		}
		break;
	}

	terminal_set_cursor(get_offset(terminal_column, terminal_row));
}

void terminal_write(const char* data, uint32_t size) {
	for (uint32_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_set_cursor(uint32_t pos) {
	pos /= 2;
	outb(VGA_COMMAND_PORT, VGA_HIGH_BYTE_COMMAND);
	outb(VGA_DATA_PORT, (pos >> 8) & 0x00FF);
	outb(VGA_COMMAND_PORT, VGA_LOW_BYTE_COMMAND);
	outb(VGA_DATA_PORT, pos & 0x00FF);
}


uint32_t terminal_get_cursor() {
	outb(VGA_COMMAND_PORT, VGA_HIGH_BYTE_COMMAND);
	uint32_t offset = (uint32_t)(inb(VGA_DATA_PORT) << 8);
	outb(VGA_COMMAND_PORT, VGA_LOW_BYTE_COMMAND);
	offset += inb(VGA_DATA_PORT);
	return offset * 2;
}