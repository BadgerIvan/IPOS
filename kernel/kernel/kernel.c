#include <stdio.h>
#include <string.h>

#include <arch/cpu/gdt.h>
#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>
#include <debug/debug.h>
#include <kernel/panic.h>

void kernel_main(void) {

	terminal_initialize();
	init_serial(SERIAL_COM1);
	debug("Terminal: successfully\n");
	debug("Serial: successfully\n");

	init_gdt();
	debug("GDT: successfully\n");

	isr_install();
	debug("IDT: successfully\n");

	init_keyboard();

	while(1) {
		asm volatile("hlt");
	}
}
