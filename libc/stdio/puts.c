#include <stdio.h>

int puts(const char *string) {
    return fputs(string, stdout);
}