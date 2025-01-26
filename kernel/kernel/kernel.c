#include <stdio.h>
#include <string.h>

#include <multiboot.h>
#include <arch/cpu/gdt.h>
#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>
#include <debug/debug.h>
#include <kernel/panic.h>
#include <kernel/syscall.h> 
#include <kernel/read_write.h>

void kernel_main(multiboot_info_t *mbd , uint32_t magic) {

	terminal_initialize();
	init_serial(SERIAL_COM1);
    init_streams();

	debug("Terminal: successfully\n");
	debug("Serial: successfully\n");
    debug("Read and write: successfully\n");

	init_gdt();
	debug("GDT: successfully\n");

	isr_install();
	debug("IDT: successfully\n");

    init_syscalls();
    debug("Syscalls: successfully\n");

	init_timer(11931);
    debug("Timer: successfully\n");

	init_keyboard();
    debug("Keyboard: successfully\n");

	printf("hello^%d\n", 10);

	while(1) {
		asm volatile("hlt");
	}
}
