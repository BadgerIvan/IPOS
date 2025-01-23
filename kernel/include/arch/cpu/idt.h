#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

void set_idt_gate(int n, uint32_t handler);

void load_idt();

#endif