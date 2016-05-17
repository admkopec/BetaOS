//
//  string.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void itoa(char *buf, unsigned long int n, int base);
size_t strlen(const char*);
int strcmp(const char* str1, const char* str2);

#ifdef __cplusplus
}
#endif

#endif /* string_h */
