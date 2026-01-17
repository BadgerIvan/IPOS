#include <stdio.h>
#include <ctype.h>

#include <multiboot.h>

#include <kernel/panic.h>

#include <arch/cpu/gdt.h>
#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/memory/paging.h>
#include <arch/memory/frames.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>

#include <debug/debug.h>

void kernel_main(multiboot_info_t* mbd, void* page_dir, void* first_page_table) {

	init_terminal();
	debug("Terminal: successfully\n");

    init_gdt();
    debug("GDT: successfully\n");
    
	init_isr();
	debug("IDT: successfully\n");

	init_timer(11931);
    debug("Timer: successfully\n");

	init_paging(page_dir, first_page_table);
	debug("Paging first 4MB: successfully\n");

	init_frames(mbd);
	debug("Frames: successfully\n");

	init_keyboard();
    debug("Keyboard: successfully\n");

	while(1) {
		asm volatile("hlt");
	}

}
