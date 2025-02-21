#include <stdio.h>
#include <ctype.h>

#include <multiboot.h>

#include <kernel/panic.h>
#include <kernel/syscall.h> 
#include <kernel/read_write.h>

#include <arch/cpu/gdt.h>
#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>
#include <arch/memory/heap.h>

#include <debug/debug.h>

void kernel_main(multiboot_info_t* mbd) {

	init_terminal();
    init_streams(); 

	debug("Terminal: successfully\n");
    debug("Read and write: successfully\n");

    init_gdt();
    debug("GDT: successfully\n");
    
	init_isr();
	debug("IDT: successfully\n");

    init_syscalls();
    debug("Syscalls: successfully\n");

	init_timer(11931);
    debug("Timer: successfully\n");

	init_keyboard();
    debug("Keyboard: successfully\n");

	while(1) {
		asm volatile("hlt");
	}

}
