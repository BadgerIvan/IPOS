#ifndef _ASSERT_H
#define _ASSERT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef assert
#ifdef NDEBUG
#define assert(ignore)((void) 0)
#else

#define CALCULATING(EX) (EX)
#ifdef __is_libk
#define assert(EX) if(!(CALCULATING(EX))) { fprintf(stderr, "\nassert fail: %s %d\n", __FILE__, __LINE__); panic("FAILED ASSERT\n"); }
#else
#define assert(EX) if(!(CALCULATING(EX))) { fprintf(stderr, "\nassert fail: %s %d\n", __FILE__, __LINE__); abort(); }
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif