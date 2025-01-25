#include <stdio.h>

#if defined(__is_libk)
#include <arch/drivers/tty.h>
#endif

int fputc(int ch, FILE *stream) {
#if defined(__is_libk)
	int result = write(stream, (char)ch, 1);
    if(result == -1) {
        return EOF;
    }
#else
	
#endif
	return ch;
}
