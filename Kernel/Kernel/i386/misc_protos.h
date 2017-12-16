//
//  misc_protos.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef misc_protos_h
#define misc_protos_h

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/cdefs.h>
#include <i386/vm_types.h>
#include <platform/boot.h>

#if DEBUG
#define DBG(x...)   printf(x)
#else
#define DBG(x...)
#endif

#define PROT_MODE_START 0x800
#define REAL_MODE_BOOTSTRAP_OFFSET 0x2000

#define    __probable(x)    __builtin_expect(!!((long)(x)), 1L)
#define    __improbable(x)    __builtin_expect(!!((long)(x)), 0L)

extern bool can_use_serial;
extern bool is_new_paging_enabled;
extern void panic(const char* errormsg, ...);
extern void kernelMain(void);
extern void enable_sse(void);
extern uint64_t tmrCvt(uint64_t time, uint64_t conversion);
extern void i386_vm_init(uint64_t maxmem, bool IA32e, boot_args	*args);
extern vm_offset_t io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags);

extern int master_cpu;

#define cpu_number() get_cpu_number()

#endif /* misc_protos_h */
