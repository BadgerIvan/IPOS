#include <stdio.h>
#include <string.h>

#if defined(__is_libk)
#include <drivers/tty.h>
#endif

int puts(const char* string) {
#if defined(__is_libk)
	terminal_write(string, strlen(string));
#else
	//libc
#endif
	return '\n';
}
