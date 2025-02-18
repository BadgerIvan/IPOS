#include <stddef.h>

#include <arch/cpu/isr.h>
#include <kernel/read_write.h>

void write_handler(registers_t *r) {
    write((int)r->eax, (const void*)r->ebx, (size_t)r->ecx);
}