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
#include <sys/cdefs.h>
#include <i386/vm_types.h>
#include <platform/boot.h>

#if DEBUG
#define DBG(x...)   kprintf(x)
#else
#define DBG(x...)
#endif

extern bool can_use_serial;
extern bool is_new_paging_enabled;
extern void kprintf(const char *fmt, ...);
extern void panic(const char* errormsg, ...);
extern void kernel_main();
extern void bcopy(const void *from, void *to, unsigned int count);
extern void bzero(void * addr, size_t length);
extern uint64_t tmrCvt(uint64_t time, uint64_t conversion);
extern void i386_vm_init(uint64_t maxmem, bool IA32e, boot_args	*args);
extern vm_offset_t io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags);

extern int master_cpu;

#define cpu_number() get_cpu_number()

#endif /* misc_protos_h */
