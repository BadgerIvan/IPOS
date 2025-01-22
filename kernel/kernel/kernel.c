#include <stdio.h>
#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>
#include <debug/debug.h>
#include <kernel/panic.h>

void kernel_main(void) {

	init_serial(SERIAL_COM1);

	terminal_initialize();
	debug("Terminal: successfully\n");

	isr_install();
	debug("ISR: successfully\n");

	init_keyboard();
	
	isr_run();
	debug("ISR: run\n");
	
	isr0();

}
