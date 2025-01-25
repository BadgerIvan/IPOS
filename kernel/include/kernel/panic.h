#ifndef _PANIC_H
#define _PANIC_H

__attribute__((__noreturn__))
void panic(const char* text);

#endif