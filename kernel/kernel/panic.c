#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/vga.h>
#include <arch/drivers/serial.h>
#include <arch/cpu/isr.h>

const char* kernel_panic_str = "\nKernel panic!\n";

__attribute__((__noreturn__))
void panic() {
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
    terminal_write(kernel_panic_str, strlen(kernel_panic_str));
    serial_write_string(kernel_panic_str, SERIAL_COM1);
    isr_stop();
    for(;;) { }
    __builtin_unreachable();
}

