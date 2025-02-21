#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

extern uint32_t KERNEL_START; 
extern uint32_t KERNEL_END;

void init_gdt();

#endif