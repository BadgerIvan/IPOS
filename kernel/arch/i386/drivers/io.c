#include <stdint.h>

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile ("inw %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %%ax, %%dx" : : "a" (data), "d" (port));
}

uint32_t inl(uint16_t port) {
    uint32_t result;
    asm volatile ("inl %%dx, %%eax" : "=a" (result) : "d" (port));
    return result;
}

void outl(uint16_t port, uint32_t data) {
    asm volatile ("outl %%eax, %%dx" : : "a" (data), "d" (port));
}

void io_wait() {
    outb(0x80, 0);
}