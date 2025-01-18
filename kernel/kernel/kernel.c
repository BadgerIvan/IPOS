#include <stdio.h>
#include <string.h>

#include <drivers/tty.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>

void kernel_main(void) {
	terminal_initialize();
	puts("Terminal: successfully\n");

	isr_install();
	puts("ISR: successfully\n");

	//init_timer(100);

    init_keyboard();

	isr_run();

	// while (1) {
    //     if (inb(0x64) & 1) {  // Проверяем, установлен ли бит 0 (данные готовы)
    //         uint8_t scancode = inb(0x60);  // Считываем скан-код из порта данных
    //         printf("%x", scancode);
    //     }
	// 	for(int i = 0; i < 100000; i++) { }
    // }
}
