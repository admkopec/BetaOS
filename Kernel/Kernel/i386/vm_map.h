//
//  vm_map.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef vm_map_h
#define vm_map_h

#include <i386/vm_types.h>
#include <i386/locks.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/cdefs.h>

struct vm_map_store {
    RB_ENTRY(vm_map_store) entry;
};

typedef struct vm_map_entry	*vm_map_entry_t;
#define VM_MAP_ENTRY_NULL	((vm_map_entry_t) 0)

typedef union vm_map_object {
    //vm_object_t		vmo_object;	/* object object */
    vm_map_t		vmo_submap;	/* belongs to another map */
} vm_map_object_t;

struct vm_map_links {
    struct vm_map_entry	*prev;		/* previous entry */
    struct vm_map_entry	*next;		/* next entry */
    vm_map_offset_t		start;		/* start address */
    vm_map_offset_t		end;		/* end address */
};

struct vm_map_header {
    struct vm_map_links	links;		/* first, last, min, max */
    int                 nentries;	/* Number of entries */
    boolean_t           entries_pageable;
    /* are map entries pageable? */
    vm_map_offset_t		highest_entry_end_addr;	/* The ending address of the highest allocated vm_entry_t */
    //struct rb_head      rb_head_store;
    int                 page_shift;	/* page shift */
};

struct vm_map_entry {
    struct vm_map_links	links;		/* links to other entries */
#define vme_prev		links.prev
#define vme_next		links.next
#define vme_start		links.start
#define vme_end			links.end
    
    struct vm_map_store	store;
    union vm_map_object	vme_object;	/* object I point to */
    vm_object_offset_t	vme_offset;	/* offset into object */
    
    unsigned int
    /* boolean_t */	is_shared:1,	/* region is shared */
    /* boolean_t */	is_sub_map:1,	/* Is "object" a submap? */
    /* boolean_t */	in_transition:1, /* Entry being changed */
    /* boolean_t */	needs_wakeup:1,	/* Waiters on in_transition */
    /* vm_behavior_t */ behavior:2,	/* user paging behavior hint */
    /* behavior is not defined for submap type */
    /* boolean_t */	needs_copy:1,	/* object need to be copied? */
    
    /* Only in task maps: */
    /* vm_prot_t */	protection:3,	/* protection code */
    /* vm_prot_t */	max_protection:3, /* maximum protection */
    /* vm_inherit_t */ inheritance:2, /* inheritance */
    /* boolean_t */	use_pmap:1,	/*
                                 * use_pmap is overloaded:
                                 * if "is_sub_map":
                                 * 	use a nested pmap?
                                 * else (i.e. if object):
                                 * 	use pmap accounting
                                 * 	for footprint?
                                 */
    /* boolean_t */	no_cache:1,	/* should new pages be cached? */
    /* boolean_t */	permanent:1,	/* mapping can not be removed */
    /* boolean_t */	superpage_size:1, /* use superpages of a certain size */
    /* boolean_t */	map_aligned:1,	/* align to map's page size */
    /* boolean_t */	zero_wired_pages:1, /* zero out the wired pages of
                                         * this entry it is being deleted
                                         * without unwiring them */
    /* boolean_t */	used_for_jit:1,
    /* boolean_t */	from_reserved_zone:1, /* Allocated from
                                           * kernel reserved zone	 */
    
    /* iokit accounting: use the virtual size rather than resident size: */
    /* boolean_t */ iokit_acct:1,
    /* boolean_t */ vme_resilient_codesign:1,
    /* boolean_t */ vme_resilient_media:1,
    __unused:6;
    ;
    
    unsigned short		wired_count;	/* can be paged if = 0 */
    unsigned short		user_wired_count; /* for vm_wire */
#if	DEBUG
#define	MAP_ENTRY_CREATION_DEBUG (1)
#define MAP_ENTRY_INSERTION_DEBUG (1)
#endif	
#if	MAP_ENTRY_CREATION_DEBUG
    struct vm_map_header	*vme_creation_maphdr;
    uintptr_t		vme_creation_bt[16];
#endif
#if	MAP_ENTRY_INSERTION_DEBUG
    uintptr_t		vme_insertion_bt[16];
#endif
};

struct _vm_map {
    lck_rw_t                lock;		/* map lock */
    struct vm_map_header	hdr;		/* Map entry header */
#define min_offset		hdr.links.start	/* start of range */
#define max_offset		hdr.links.end	/* end of range */
#define highest_entry_end	hdr.highest_entry_end_addr
    pmap_t              pmap;		/* Physical map */
    vm_map_size_t		size;		/* virtual size */
    vm_map_size_t		user_wire_limit;/* rlimit on user locked memory */
    vm_map_size_t		user_wire_size; /* current size of user locked memory in this map */
    int			ref_count;	/* Reference count */
#if	TASK_SWAPPER
    int			res_count;	/* Residence count (swap) */
    int			sw_state;	/* Swap state */
#endif	/* TASK_SWAPPER */
    //decl_lck_mtx_data(,	s_lock)		/* Lock ref, res fields */
    lck_mtx_ext_t		s_lock_ext;
    vm_map_entry_t		hint;		/* hint for quick lookups */
    struct vm_map_links*	hole_hint;	/* hint for quick hole lookups */
    union{
        vm_map_entry_t		_first_free;	/* First free space hint */
        struct vm_map_links*	_holes;		/* links all holes between entries */
    }f_s;						/* Union for free space data structures being used */
    
#define first_free		f_s._first_free
#define holes_list		f_s._holes
    
    unsigned int
    /* boolean_t */		wait_for_space:1, /* Should callers wait for space? */
    /* boolean_t */		wiring_required:1, /* All memory wired? */
    /* boolean_t */		no_zero_fill:1, /*No zero fill absent pages */
    /* boolean_t */		mapped_in_other_pmaps:1, /*has this submap been mapped in maps that use a different pmap */
    /* boolean_t */		switch_protect:1, /*  Protect map from write faults while switched */
    /* boolean_t */		disable_vmentry_reuse:1, /*  All vm entries should keep using newer and higher addresses in the map */
    /* boolean_t */		map_disallow_data_exec:1, /* Disallow execution from data pages on exec-permissive architectures */
    /* boolean_t */		holelistenabled:1,
    /* reserved */		pad:24;
    unsigned int		timestamp;	/* Version number */
    unsigned int		color_rr;	/* next color (not protected by a lock) */
#if CONFIG_FREEZE
    void			*default_freezer_handle;
#endif
    boolean_t		jit_entry_exists;
} ;

#endif /* vm_map_h */
