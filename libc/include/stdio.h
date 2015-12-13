#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void printf(const char*, ...);
int putchar(int);
void puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
