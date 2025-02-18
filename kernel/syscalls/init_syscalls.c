#include <kernel/syscall.h>

#include <arch/cpu/isr.h>

void write_handler(registers_t *reg);

void init_syscalls() {
    register_interrupt_handler(IRQ16, write_handler);
}