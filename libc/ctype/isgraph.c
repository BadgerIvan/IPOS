#include <ctype.h>

int isgraph(int c) {
    return !iscntrl(c) && (char)c != ' ';
}