#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/vga.h>
#include <arch/drivers/serial.h>
#include <arch/cpu/isr.h>
#include <debug/debug.h>

__attribute__((__noreturn__))
void panic(const char* text) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
    debugf("\nKernel panic\n%s", text);
    isr_stop();
    for(;;) { }
    __builtin_unreachable();
}

