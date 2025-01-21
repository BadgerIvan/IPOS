#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/serial.h>

const char* kernel_panic_str = "\nKernel panic!\n";

__attribute__((__noreturn__))
void kernel_panic() {
    terminal_write(kernel_panic_str, strlen(kernel_panic_str));
    serial_write_string(kernel_panic_str, SERIAL_COM1);
    for(;;);
    __builtin_unreachable();
}

