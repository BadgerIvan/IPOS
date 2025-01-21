#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/serial.h>

void debug(const char* str) {
#ifdef _DEBUG
    terminal_write(str, strlen(str));
#endif
    serial_write_string(str, SERIAL_COM1);
    serial_write_char('\r', SERIAL_COM1);
}