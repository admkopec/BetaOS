//
//  kalloc.c
//  BetaOS
//
//  Created by Adam Kopeć on 5/14/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <kernel/kalloc.h>
#include <i386/vm_types.h>
#include <i386/vm_map.h>
#include <stddef.h>
#include "misc_protos.h"

#define KALLOC_MAP_SIZE_MIN  (16  * 1024 * 1024)
#define KALLOC_MAP_SIZE_MAX  (128 * 1024 * 1024)

vm_map_t  kalloc_map;
vm_size_t kalloc_max;
vm_size_t kalloc_max_prerounded;
vm_size_t kalloc_kernmap_size;	/* size of kallocs that can come from kernel map */

/* how many times we couldn't allocate out of kalloc_map and fell back to kernel_map */
unsigned long kalloc_fallback_count;

unsigned int kalloc_large_inuse;
vm_size_t    kalloc_large_total;
vm_size_t    kalloc_large_max;
vm_size_t    kalloc_largest_allocated = 0;
uint64_t     kalloc_large_sum;

int	kalloc_fake_zone_index = -1; /* index of our fake zone in statistics arrays */

vm_offset_t	kalloc_map_min;
vm_offset_t	kalloc_map_max;