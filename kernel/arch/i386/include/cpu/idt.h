#include <stdint.h>

#ifndef _IDT_H
#define _IDT_H 1

void set_idt_gate(int n, uint32_t handler);

void load_idt();

#endif