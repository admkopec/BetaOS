//
//  mp_desc.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef mp_desc_h
#define mp_desc_h

#include <stdint.h>
#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/cpu_data.h>
#include <sys/cdefs.h>

/*
 * The descriptor tables are together in a structure
 * allocated one per processor (except for the boot processor).
 */
typedef struct cpu_desc_table64 {
    struct fake_descriptor	gdt[GDTSZ]       __attribute__ ((aligned (16)));
    struct x86_64_tss       ktss             __attribute__ ((aligned (16)));
    struct sysenter_stack	sstk	         __attribute__ ((aligned (16)));
    uint8_t                 fstk[PAGE_SIZE]  __attribute__ ((aligned (16)));
} cpu_desc_table64_t;

#define	current_gdt()	(current_cpu_datap()->cpu_desc_index.cdi_gdt.ptr)
#define	current_idt()	(current_cpu_datap()->cpu_desc_index.cdi_idt.ptr)
#define	current_ldt()	(current_cpu_datap()->cpu_desc_index.cdi_ldt)
#define	current_ktss()	(current_cpu_datap()->cpu_desc_index.cdi_ktss)
#define	current_sstk()	(current_cpu_datap()->cpu_desc_index.cdi_sstk)

#define	current_ktss64() ((struct x86_64_tss *) current_ktss())
#define	current_sstk64() ((addr64_t *) current_sstk())

#define	gdt_desc_p(sel) \
(&((struct real_descriptor *)current_gdt())[sel_idx(sel)])
#define	ldt_desc_p(sel) \
(&((struct real_descriptor *)current_ldt())[sel_idx(sel)])

void	cpu_mode_init(cpu_data_t *cdp);

void	cpu_desc_init(cpu_data_t *cdp);
void	cpu_desc_init64(cpu_data_t *cdp);
void	cpu_desc_load(cpu_data_t *cdp);
void	cpu_desc_load64(cpu_data_t *cdp);

bool
valid_user_data_selector(uint16_t selector);

bool
valid_user_code_selector(uint16_t selector);

bool
valid_user_stack_selector(uint16_t selector);

bool
valid_user_segment_selectors(uint16_t cs,
                             uint16_t ss,
                             uint16_t ds,
                             uint16_t es,
                             uint16_t fs,
                             uint16_t gs);

#endif /* mp_desc_h */
