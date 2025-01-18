#include <stdint.h>
#include <stdio.h>

#include <cpu/isr.h>
#include <drivers/io.h>
#include <drivers/keyboard.h>

static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = inb(0x60);
    printf("%x\n", scancode);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}