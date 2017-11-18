//
//  stdio.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int    printf   (const char*, ...);
int    vasprintf(char **strp, const char * format, va_list argp);
int    asprintf (char **strp, const char * format, ...);
//int    snprintf (char * buf, size_t size,  const char * format, ...);
size_t write    (int fd, const void *buf, size_t nbyte);
int    putchar  (int);
int    puts     (const char*);
int    getchar  (void);
char*  gets     (char*);

#ifdef __cplusplus
}
#endif

#endif /* stdio_h */
