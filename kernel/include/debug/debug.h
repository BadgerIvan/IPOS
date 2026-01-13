#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef NDEBUG
#define printk(...) ((void)0)
#else
int printk(const char* format, ...);
#endif

#ifdef NDEBUG
#define debug(str) ((void)0)
#define debugf(...) ((void)0)
#else 
#define debug(str) printk("%s", str)
#define debugf(...) printk(__VA_ARGS__)
#endif

#endif