#include <stddef.h>

#include <arch/cpu/isr.h>

void write_handler(registers_t *r) {
    write((int)r->eax, (const void*)r->ebx, (size_t)r->ecx);
}