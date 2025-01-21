#ifndef _DEBUG_H
#define _DEBUG_H 1

__attribute__((__noreturn__))
void kernel_panic();

void debug(const char* str);

#endif