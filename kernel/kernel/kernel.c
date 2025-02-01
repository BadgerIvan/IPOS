#include <stdio.h>
#include <string.h>
#include <assert.h>

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
#include <arch/memory/paging.h>
#include <arch/memory/heap.h>

extern uint32_t KERNEL_START; 
extern uint32_t KERNEL_END;

void kernel_main(multiboot_info_t *mbd , uint32_t magic) {

    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }

	terminal_initialize();
    init_streams();

	debug("Terminal: successfully\n");
    debug("Read and write: successfully\n");

    debugf("Kernel starts at: 0x%x\n", (uint32_t)&KERNEL_START);
    debugf("Kernel ends at: 0x%x\n", (uint32_t)&KERNEL_END);
    debugf("Kernel size: %d bytes\n", (uint32_t)&KERNEL_END - (uint32_t)&KERNEL_START);

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

	while(1) {
		asm volatile("hlt");
	}
}
