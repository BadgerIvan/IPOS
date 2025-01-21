#include <stdio.h>
#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/cpu/idt.h>
#include <arch/cpu/isr.h>
#include <arch/drivers/keyboard.h>
#include <arch/drivers/timer.h>
#include <arch/drivers/serial.h>
#include <debug/debug.h>

void kernel_main(void) {

	init_serial(SERIAL_COM1);

	terminal_initialize();
	debug("Terminal: successfully\n");

	isr_install();
	debug("ISR: successfully\n");

	init_keyboard();
	
	isr_run();
	debug("ISR: run\n");

	// while (1) {
    //     if (inb(0x64) & 1) {  // Проверяем, установлен ли бит 0 (данные готовы)
    //         uint8_t scancode = inb(0x60);  // Считываем скан-код из порта данных
    //         printf("%x", scancode);
    //     }
	// 	for(int i = 0; i < 100000; i++) { }
    // }
}
