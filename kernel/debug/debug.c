#include <string.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/serial.h>

void debug(const char* str) {
#ifdef _DEBUG
    puts(str);
#endif
}