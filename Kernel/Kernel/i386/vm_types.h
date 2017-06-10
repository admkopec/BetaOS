//
//  vm_types.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/28/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef vm_types_h
#define vm_types_h

/*
 * A vm_offset_t is a type-neutral pointer,
 * e.g. an offset into a virtual memory space.
 */
#ifdef __LP64__
typedef unsigned long		vm_offset_t;
#else	/* __LP64__ */
typedef	unsigned int		vm_offset_t;
#endif	/* __LP64__ */

/*
 * A vm_size_t is the proper type for e.g.
 * expressing the difference between two
 * vm_offset_t entities.
 */
#ifdef __LP64__
typedef unsigned long		vm_size_t;
#else	/* __LP64__ */
typedef	unsigned int		vm_size_t;
#endif	/* __LP64__ */

/*
 * To minimize the use of 64-bit fields, we keep some physical
 * addresses (that are page aligned) as 32-bit page numbers.
 * This limits the physical address space to 16TB of RAM.
 */

typedef uint32_t ppnum_t;		/* Physical page number */
#define PPNUM_MAX UINT32_MAX

/*
 * This new type is independent of a particular vm map's
 * implementation size - and represents appropriate types
 * for all possible maps.  This is used for interfaces
 * where the size of the map is not known - or we don't
 * want to have to distinguish.
 */

typedef uint64_t		vm_map_offset_t;
typedef uint64_t		vm_map_address_t;
typedef uint64_t		vm_map_size_t;

typedef vm_offset_t     vm_address_t;

typedef struct pmap                 *pmap_t;
typedef struct _vm_map              *vm_map_t;
typedef struct vm_object            *vm_object_t;
//typedef struct vm_object_fault_info *vm_object_fault_info_t;

typedef uint64_t		vm_object_offset_t;
typedef uint64_t		vm_object_size_t;

#define VM_TAG_BT	0x00000001
#define VM_TAG_KMOD	0x00000002
#define VM_TAG_UNLOAD	0x00000004
typedef uint8_t vm_tag_t;
struct vm_allocation_site {
    vm_tag_t tag;
    uint8_t  flags;
};
typedef struct vm_allocation_site vm_allocation_site_t;

#define PMAP_NULL		((pmap_t) 0)
#define VM_OBJECT_NULL	((vm_object_t) 0)

typedef uint32_t		ast_t; // Temporary


#endif /* vm_types_h */
