//
//  mp.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef mp_h
#define mp_h

#include <i386/apic.h>

#define MAX_CPUS 64

#ifndef __ASSEMBLY__
#include <stdint.h>
#include <sys/cdefs.h>
#include <kernel/kern_return.h>

void cpu_init(void); // Temp

kern_return_t intel_startCPU(int slot_number);
kern_return_t intel_startCPU_fast(int slot_number);
void          i386_init_slave(void);
void          i386_init_slave_fast(void);
void          smp_init(void);
void          cpu_interrupt(int cpu);

extern unsigned int real_ncpus;    // Real number of CPUs
extern unsigned int max_ncpus;     // Max  number of CPUs

#endif /* __ASSEMBLY__ */

#endif /* mp_h */
