#ifndef _DEBUG_H
#define _DEBUG_H

int printk(const char* format, ...);

#ifdef NDEBUG
#define debug(str) ((void)0)
#define debugf(...) ((void)0)
#else 
#define debug(str) printk("%s", str)
#define debugf(...) printk(__VA_ARGS__)
#endif

#ifdef NDEBUG
#define assertk(exp) ((void)0)
#else
#include <kernel/panic.h>
#define CALCULATING_EXP(exp) (exp)
#define assertk(exp) if(!CALCULATING_EXP(exp)) \
        {printk("ASSERT FAILED: (%s), file: %s, line: %d", #exp, __FILE__, __LINE__); \
        panic("assert failed");}
#endif

#endif