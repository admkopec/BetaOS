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
#include <sys/cdefs.h>

extern bool can_use_serial;
extern bool is_new_paging_enabled;
extern void kprintf(const char* fmt, ...);
extern void panic(const char* errormsg, ...);
extern void kernel_main(void);
extern void bcopy(const void *from, void *to, unsigned int count);
extern void bzero(void * addr, size_t length);
extern uint64_t tmrCvt(uint64_t time, uint64_t conversion);

#endif /* misc_protos_h */
