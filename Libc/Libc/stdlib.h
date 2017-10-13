//
//  stdlib.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif
    
// Default FILE * for stdio, faked values
// OSX symbol names
extern void *__stderrp;
extern void *__stdinp;
extern void *__stdoutp;
    
// Linux symbol names
extern void *stderr;
extern void *stdin;
extern void *stdout;
    
typedef struct Link {
    struct Link *prev;
    struct Link *next;
} Link;
    
void* malloc(size_t size);
void  free(void* ptr);

//__attribute__((__noreturn__))
//void panic(const char* errormsg);
void error(const char* errormsg);

#ifdef __cplusplus
}
#endif

#endif /* stdlib_h */
