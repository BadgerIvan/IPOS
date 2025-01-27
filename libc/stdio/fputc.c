#include <stdio.h>

#if defined(__is_libk)
#include <kernel/read_write.h>
#endif

int fputc(int ch, FILE *stream) {
    char c = (char)ch;
#if defined(__is_libk)
	int result = write(stream->id, &c, 1);
    if(result == -1) {
        return EOF;
    }
#else

#endif
	return ch;
}
