//
//  stdlib.h
//  OS
//
//  Created by Adam Kopeć on 12/7/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

//__attribute__((__noreturn__))
void kpanic(const char* errormsg);

#ifdef __cplusplus
}
#endif

#endif /* stdlib_h */
