#include <arch/cpu/isr.h>
#include <debug/debug.h>

__attribute__((__noreturn__))
void panic(const char *text) {
    isr_stop();
    debugf("Kernel panic! %s\n", text);
    for(;;);
    __builtin_unreachable();
}
