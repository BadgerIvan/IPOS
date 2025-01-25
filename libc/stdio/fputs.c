#include <stdio.h>
#include <string.h>

#if defined(__is_libk)
#include <kernel/read_write.h>
#endif

int fputs(const char* string, FILE *stream) {
#if defined(__is_libk)
    int result = write(stream, string, strlen(string));
    if(result == -1) {
        return EOF;
    }
#else

#endif
	return 1;
}
