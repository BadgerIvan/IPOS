#include <stdint.h>

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile ("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile ("out %%ax, %%dx" : : "a" (data), "d" (port));
}

uint32_t indw(uint16_t port) {
    uint32_t result;
    asm volatile ("in %%dx, %%eax" : "=a" (result) : "d" (port));
    return result;
}

void outdw(uint16_t port, uint32_t data) {
    asm volatile ("out %%eax, %%dx" : : "a" (data), "d" (port));
}

void io_wait() {
    outb(0x80, 0);
}