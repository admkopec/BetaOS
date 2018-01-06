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
#include <i386/vm_param.h>
#include <stddef.h>
#include <i386/misc_protos.h>

#define KALLOC_MAP_SIZE_MIN  (16  * 1024 * 1024)
#define KALLOC_MAP_SIZE_MAX  (128 * 1024 * 1024)

extern uint64_t first_avail;
extern vm_offset_t virtual_avail;
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

// Simple allocation and free methods that need to be changed as soon as possible

void * kalloc_(uint32_t size) {
	uint64_t res;
	res = (uint64_t) io_map(first_avail, round_page(size), 0);
	first_avail += round_page(size);
	return (void *) res;
}

void free_(void * data, uint32_t size) {
    if (data + round_page(size) == (void *) virtual_avail) {
        first_avail -= round_page(size);
        virtual_avail -= round_page(size);
    }
}
