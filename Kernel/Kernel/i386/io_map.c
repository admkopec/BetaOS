//
//  io_map.c
//  Kernel
//
//  Created by Adam Kopeć on 2/19/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <stdint.h>
#include <i386/pmap.h>
#include <i386/vm_prot.h>
#include <i386/pmap_internal.h>
#include "misc_protos.h"
#include "vm_param.h"

extern vm_offset_t virtual_avail;
extern vm_offset_t pmap_map_bd(vm_offset_t virt, vm_map_offset_t start_addr, vm_map_offset_t end_addr, vm_prot_t prot, unsigned int flags);

vm_offset_t
io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags) {
    vm_offset_t	start;
    start = virtual_avail;
    virtual_avail += round_page(size);
    (void) pmap_map_bd(start, phys_addr, phys_addr + round_page(size), VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE, flags);
    return (start);
}
