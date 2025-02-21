#include <stdarg.h>
#include <stdio.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/vga.h>
#include <arch/drivers/serial.h>
#include <arch/cpu/isr.h>
#include <debug/debug.h>

__attribute__((__noreturn__))
void panic(const char *text) {
    isr_stop();
    debug("Kernel panic! ");
    debug(text);
    for(;;) { }
    __builtin_unreachable();
}
