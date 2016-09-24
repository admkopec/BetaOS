//
//  kalloc.h
//  BetaOS
//
//  Created by Adam Kopeć on 5/14/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef kmalloc_h
#define kmalloc_h

#include <stddef.h>
#include <stdint.h>
#include <i386/vm_types.h>
#include <sys/cdefs.h>

extern void *
kalloc_canblock(
                vm_size_t	       size,
                bool               canblock,
                vm_allocation_site_t * site);

#define kalloc(size)				\
({ static vm_allocation_site_t site __attribute__((section("__DATA, __data"))); \
kalloc_canblock((size), TRUE, &site); })

#define kalloc_tag(size, tag)			\
({ static vm_allocation_site_t site __attribute__((section("__DATA, __data"))) \
= { (tag), 0 } ; \
kalloc_canblock((size), TRUE, &site); })

#define kalloc_tag_bt(size, tag)		\
({ static vm_allocation_site_t site __attribute__((section("__DATA, __data"))) \
= { (tag), VM_TAG_BT }; \
kalloc_canblock((size), TRUE, &site); })

#define kalloc_noblock(size)			\
({ static vm_allocation_site_t site __attribute__((section("__DATA, __data"))); \
kalloc_canblock((size), FALSE, &site); })

#define kalloc_noblock_tag_bt(size, tag)	\
({ static vm_allocation_site_t site __attribute__((section("__DATA, __data"))) \
= { (tag), VM_TAG_BT }; \
kalloc_canblock((size), FALSE, &site); })

extern void kfree(void		*data, vm_size_t	size);

#endif /* kmalloc_h */
