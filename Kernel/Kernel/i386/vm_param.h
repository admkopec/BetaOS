//
//  vm_param.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/30/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef vm_param_h
#define vm_param_h

/* Kernel-wide values */

#define KB        (1024ULL)
#define MB        (1024*KB)
#define GB        (1024*MB)

#define BYTE_SIZE           8		/* byte size in bits */

#define I386_PGBYTES	    4096	/* bytes per 80386 page */
#define I386_PGSHIFT		12		/* bitshift for pages */

#define INTSTACK_SIZE        (I386_PGBYTES*4)

#define	PAGE_SIZE           I386_PGBYTES
#define	PAGE_SHIFT          I386_PGSHIFT
#define	PAGE_MASK           (PAGE_SIZE - 1)

#define PAGE_SIZE_64 (unsigned long long)PAGE_SIZE		/* pagesize in addr units */
#define PAGE_MASK_64 (unsigned long long)PAGE_MASK		/* mask for off in page */

#define PAGE_MAX_SHIFT      PAGE_SHIFT
#define PAGE_MAX_SIZE       PAGE_SIZE
#define PAGE_MAX_MASK       PAGE_MASK

#define PAGE_MIN_SHIFT      PAGE_SHIFT
#define PAGE_MIN_SIZE       PAGE_SIZE
#define PAGE_MIN_MASK       PAGE_MASK

#define	page_aligned(x)     (((x) & PAGE_MASK) == 0)

/*
 *	Convert bytes to pages and convert pages to bytes.
 *	No rounding is used.
 */

#define i386_btop(x)		((ppnum_t)((x) >> I386_PGSHIFT))
#define machine_btop(x)		i386_btop(x)
#define i386_ptob(x)		(((pmap_paddr_t)(x)) << I386_PGSHIFT)
#define machine_ptob(x)		i386_ptob(x)

#define NCOPY_WINDOWS       0

/*
 *   Convert addresses to pages and vice versa.  No rounding is used.
 *   The atop_32 and ptoa_32 macros should not be use on 64 bit types.
 *   The round_page_64 and trunc_page_64 macros should be used instead.
 */

#define atop_32(x) ((uint32_t)(x) >> PAGE_SHIFT)
#define ptoa_32(x) ((uint32_t)(x) << PAGE_SHIFT)
#define atop_64(x) ((uint64_t)(x) >> PAGE_SHIFT)
#define ptoa_64(x) ((uint64_t)(x) << PAGE_SHIFT)

#define atop_kernel(x) ((vm_address_t)(x) >> PAGE_SHIFT)
#define ptoa_kernel(x) ((vm_address_t)(x) << PAGE_SHIFT)

#define round_page(x) (((vm_offset_t)(x) + PAGE_MASK) & ~((vm_offset_t)PAGE_MASK))
#define trunc_page(x) ((vm_offset_t)(x) & ~((vm_offset_t)PAGE_MASK))

#define round_page_64(x) (((uint64_t)(x) + PAGE_MASK_64) & ~((uint64_t)PAGE_MASK_64))
#define trunc_page_64(x) ((uint64_t)(x) & ~((uint64_t)PAGE_MASK_64))

/*
 * Maximum physical memory supported.
 */
#define	K32_MAXMEM      (32*GB)
#define	K64_MAXMEM      (252*GB)
#define KERNEL_MAXMEM	K64_MAXMEM

#define KERNEL_IMAGE_TO_PHYS(x)         (x)
#define VM_MIN_KERNEL_ADDRESS           ((vm_offset_t) 0xFFFFFF8000000000UL)
#define VM_MIN_KERNEL_PAGE              ((ppnum_t)0)
#define VM_MIN_KERNEL_AND_KEXT_ADDRESS	(VM_MIN_KERNEL_ADDRESS - 0x80000000ULL)
#define VM_MAX_KERNEL_ADDRESS           ((vm_offset_t) 0xFFFFFFFFFFFFEFFFUL)
#define VM_MAX_KERNEL_ADDRESS_EFI32     ((vm_offset_t) 0xFFFFFF80FFFFEFFFUL)

#ifndef __ASSEMBLY__
#include <mach/vm_types.h>
extern vm_offset_t vm_last_addr;

extern const vm_offset_t    vm_min_kernel_address;
extern const vm_offset_t    vm_max_kernel_address;

extern vm_offset_t        vm_kernel_stext;
extern vm_offset_t        vm_kernel_etext;
extern vm_offset_t        vm_kernel_base;
extern vm_offset_t        vm_kernel_top;
extern vm_offset_t        vm_kernel_slide;
extern vm_offset_t        vm_hib_base;
extern vm_offset_t        vm_kernel_addrperm;

extern vm_offset_t        vm_kext_base;
extern vm_offset_t        vm_kext_top;
extern vm_offset_t      vm_prelink_stext;
extern vm_offset_t      vm_prelink_etext;
extern vm_offset_t      vm_prelink_sinfo;
extern vm_offset_t      vm_prelink_einfo;
extern vm_offset_t      vm_slinkedit;
extern vm_offset_t      vm_elinkedit;

#define VM_KERNEL_IS_SLID(_o)                               \
(((vm_offset_t)(_o) >= vm_kernel_base) &&               \
((vm_offset_t)(_o) <=  vm_kernel_top))
#define VM_KERNEL_IS_KEXT(_o)      \
(((vm_offset_t)(_o) >= vm_kext_base) &&   \
((vm_offset_t)(_o) <  vm_kext_top))

#define VM_KERNEL_IS_PRELINKTEXT(_o)        \
(((vm_offset_t)(_o) >= vm_prelink_stext) &&     \
((vm_offset_t)(_o) <  vm_prelink_etext))

#define VM_KERNEL_IS_PRELINKINFO(_o)        \
(((vm_offset_t)(_o) >= vm_prelink_sinfo) &&     \
((vm_offset_t)(_o) <  vm_prelink_einfo))

#define VM_KERNEL_IS_KEXT_LINKEDIT(_o)        \
(((vm_offset_t)(_o) >= vm_slinkedit) &&     \
((vm_offset_t)(_o) <  vm_elinkedit))

#define VM_KERNEL_SLIDE(_u)                               \
((vm_offset_t)(_u) + vm_kernel_slide)

#define VM_KERNEL_UNSLIDE(_v)                               \
((VM_KERNEL_IS_SLID(_v) ||                       \
VM_KERNEL_IS_KEXT(_v) ||      \
VM_KERNEL_IS_PRELINKTEXT(_v) ||   \
VM_KERNEL_IS_PRELINKINFO(_v) ||   \
VM_KERNEL_IS_KEXT_LINKEDIT(_v)) ?      \
(vm_offset_t)(_v) - vm_kernel_slide :    \
(vm_offset_t)(_v))

#endif /* __ASSEMBLY__ */
#endif /* vm_param_h */
