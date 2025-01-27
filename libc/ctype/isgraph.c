#include <ctype.h>

int isgraph(int c) {
    return !iscntrl(c) && !isblank(c);
}