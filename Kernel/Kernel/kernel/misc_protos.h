//
//  misc_protos.h
//  BetaOS
//
//  Created by Adam Kopeć on 5/16/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef misc_protos_h
#define misc_protos_h

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/cdefs.h>

#define    __probable(x)    __builtin_expect(!!((long)(x)), 1L)
#define    __improbable(x)    __builtin_expect(!!((long)(x)), 0L)

extern void panic(const char* errormsg, ...);
extern void kernelMain(void);
extern uint64_t tmrCvt(uint64_t time, uint64_t conversion);

#endif /* misc_protos_h */
