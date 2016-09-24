//
//  lapic.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "lapic.h"
#include "mp.h"

lapic_ops_table_t       *lapic_ops;     /* Lapic operations switch */
//static vm_map_offset_t	lapic_pbase;	/* Physical base memory-mapped regs */
//static vm_offset_t      lapic_vbase;	/* Virtual base memory-mapped regs */

int		lapic_to_cpu[MAX_LAPICIDS];
int		cpu_to_lapic[MAX_CPUS];