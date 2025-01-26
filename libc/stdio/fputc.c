#include <stdio.h>

#if defined(__is_libk)
#include <arch/drivers/tty.h>
#endif

int fputc(int ch, FILE *stream) {
#if defined(__is_libk)
    char c = (char)ch;
	int result = write(stream, &c, 1);
    if(result == -1) {
        return EOF;
    }
#else
	
#endif
	return ch;
}
