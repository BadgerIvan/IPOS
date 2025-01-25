#include <stdio.h>

int putc(int ch) {
    return fputc(ch, stdout);
}
