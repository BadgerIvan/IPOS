#include <stdint.h>

#include <arch/cpu/idt.h>

#define IDT_ENTRIES 256
#define KERNEL_CS 0x08

#define low_16(address) (uint32_t)((address) & 0xFFFF)
#define high_16(address) (uint32_t)(((address) >> 16) & 0xFFFF)

typedef struct idt_gate {
    uint16_t low_offset;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct idt_register {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

static idt_gate_t idt[IDT_ENTRIES];
static idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
}

void load_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_register_t) - 1;
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}