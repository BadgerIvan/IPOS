#include <stdio.h>

#include <arch/drivers/tty.h>

void debug(const char* str) {
#ifdef _DEBUG
    puts(str);
#endif
}