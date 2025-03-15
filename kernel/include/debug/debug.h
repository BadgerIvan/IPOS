#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef NDEBUG
#define debug(str) ((void)0)
#define debugf(...) ((void)0)
#else 
#define debug(str) fputs(str, stderr)
#define debugf(...) fprintf(stderr, __VA_ARGS__)
#endif

#endif