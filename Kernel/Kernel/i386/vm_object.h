//
//  vm_object.h
//  Kernel
//
//  Created by Adam Kopeć on 4/24/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef vm_object_h
#define vm_object_h

struct vm_object {
    //queue_head_t		memq;		/* Resident memory */
    //lck_rw_t            Lock;		/* Synchronization */
    
    union {
        vm_object_size_t  vou_size;                 /* Object size (only valid if internal) */
        int               vou_cache_pages_to_scan;	/* Pages yet to be visited in an external object in cache */
    } vo_un1;
    
    struct      vm_page		*memq_hint;
    int			ref_count;	/* Number of references */
#if	TASK_SWAPPER
    int			res_count;	/* Residency references (swap)*/
#endif	/* TASK_SWAPPER */
    unsigned int		resident_page_count;    /* number of resident pages */
    unsigned int		wired_page_count;       /* number of wired pages */
    unsigned int		reusable_page_count;
    
    struct vm_object	*copy;		/* Object that should receive
                                     * a copy of my changed pages,
                                     * for copy_delay, or just the
                                     * temporary object that
                                     * shadows this object, for
                                     * copy_call.
                                     */
    struct vm_object	*shadow;	/* My shadow */
    
    union {
        vm_object_offset_t vou_shadow_offset;	/* Offset into shadow */
        /*clock_sec_t	vou_cache_ts;	* age of an external object present in cache *
        task_t		vou_purgeable_owner;	* If the purg'a'ble bits below are set
                                             * to volatile/emtpy, this is the task
                                             * that owns this purgeable object.
                                             */
        struct vm_shared_region_slide_info *vou_slide_info;
    } vo_un2;
    
    //memory_object_t		pager;          /* Where to get data */
    vm_object_offset_t	paging_offset;	/* Offset into memory object */
    //memory_object_control_t	pager_control;	/* Where data comes back */
    
    //memory_object_copy_strategy_t copy_strategy;	/* How to handle data copy */
    
#if __LP64__
    /*
     * Some user processes (mostly VirtualMachine software) take a large
     * number of UPLs (via IOMemoryDescriptors) to wire pages in large
     * VM objects and overflow the 16-bit "activity_in_progress" counter.
     * Since we never enforced any limit there, let's give them 32 bits
     * for backwards compatibility's sake.
     */
    unsigned int		paging_in_progress:16,
                        __object1_unused_bits:16;
    unsigned int		activity_in_progress;
#else /* __LP64__ */
    /*
     * On 32-bit platforms, enlarging "activity_in_progress" would increase
     * the size of "struct vm_object".  Since we don't know of any actual
     * overflow of these counters on these platforms, let's keep the
     * counters as 16-bit integers.
     */
    unsigned short		paging_in_progress;
    unsigned short		activity_in_progress;
#endif /* __LP64__ */
    /* The memory object ports are
     * being used (e.g., for pagein
     * or pageout) -- don't change
     * any of these fields (i.e.,
     * don't collapse, destroy or
     * terminate)
     */
    
    unsigned int
    /* boolean_t array */	all_wanted:11,	/* Bit array of "want to be
                                             * awakened" notations.  See
                                             * VM_OBJECT_EVENT_* items
                                             * below */
    /* boolean_t */	pager_created:1,	/* Has pager been created? */
    /* boolean_t */	pager_initialized:1,	/* Are fields ready to use? */
    /* boolean_t */	pager_ready:1,		/* Will pager take requests? */
    
    /* boolean_t */		pager_trusted:1,/* The pager for this object
                                         * is trusted. This is true for
                                         * all internal objects (backed
                                         * by the default pager)
                                         */
    /* boolean_t */		can_persist:1,	/* The kernel may keep the data
                                         * for this object (and rights
                                         * to the memory object) after
                                         * all address map references
                                         * are deallocated?
                                         */
    /* boolean_t */		internal:1,	/* Created by the kernel (and
                                     * therefore, managed by the
                                     * default memory manger)
                                     */
    /* boolean_t */		temporary:1,	/* Permanent objects may be
                                         * changed externally by the
                                         * memory manager, and changes
                                         * made in memory must be
                                         * reflected back to the memory
                                         * manager.  Temporary objects
                                         * lack both of these
                                         * characteristics.
                                         */
    /* boolean_t */		private:1,	/* magic device_pager object,
                                     * holds private pages only */
    /* boolean_t */		pageout:1,	/* pageout object. contains
                                     * private pages that refer to
                                     * a real memory object. */
    /* boolean_t */		alive:1,	/* Not yet terminated */
    
    /* boolean_t */		purgable:2,	/* Purgable state.  See
                                     * VM_PURGABLE_*
                                     */
    /* boolean_t */		purgeable_when_ripe:1, /* Purgeable when a token
                                                * becomes ripe.
                                                */
    /* boolean_t */		shadowed:1,	/* Shadow may exist */
    /* boolean_t */		advisory_pageout:1,
    /* Instead of sending page
     * via OOL, just notify
     * pager that the kernel
     * wants to discard it, page
     * remains in object */
    /* boolean_t */		true_share:1,
    /* This object is mapped
     * in more than one place
     * and hence cannot be
     * coalesced */
    /* boolean_t */		terminating:1,
    /* Allows vm_object_lookup
     * and vm_object_deallocate
     * to special case their
     * behavior when they are
     * called as a result of
     * page cleaning during
     * object termination
     */
    /* boolean_t */		named:1,	/* An enforces an internal
                                     * naming convention, by
                                     * calling the right routines
                                     * for allocation and
                                     * destruction, UBC references
                                     * against the vm_object are
                                     * checked.
                                     */
    /* boolean_t */		shadow_severed:1,
    /* When a permanent object
     * backing a COW goes away
     * unexpectedly.  This bit
     * allows vm_fault to return
     * an error rather than a
     * zero filled page.
     */
    /* boolean_t */		phys_contiguous:1,
    /* Memory is wired and
     * guaranteed physically
     * contiguous.  However
     * it is not device memory
     * and obeys normal virtual
     * memory rules w.r.t pmap
     * access bits.
     */
    /* boolean_t */		nophyscache:1;
    /* When mapped at the
     * pmap level, don't allow
     * primary caching. (for
     * I/O)
     */
    
    
    
    //queue_chain_t		cached_list;	/* Attachment point for the list of objects cached as a result of their can_persist value */
    
    //queue_head_t		msr_q;		/* memory object synchronise request queue */
    
    /*
     * the following fields are not protected by any locks
     * they are updated via atomic compare and swap
     */
    vm_object_offset_t	last_alloc;	/* last allocation offset */
    int			sequential;	/* sequential access size */
    
    uint32_t		pages_created;
    uint32_t		pages_used;
#if	MACH_PAGEMAP
    vm_external_map_t	existence_map;	/* bitmap of pages written to
                                         * backing storage */
#endif	/* MACH_PAGEMAP */
    vm_offset_t		cow_hint;	/* last page present in     */
    /* shadow but not in object */
#if	MACH_ASSERT
    struct vm_object	*paging_object;	/* object which pages to be
                                         * swapped out are temporary
                                         * put in current object
                                         */
#endif
    /* hold object lock when altering */
    unsigned int
                wimg_bits:8,	        /* cache WIMG bits         */
                code_signed:1,          /* pages are signed and should be validated; the signatures are stored with pager */
                hashed:1,               /* object/pager entered in hash */
                transposed:1,           /* object was transposed with another */
                mapping_in_progress:1,	/* pager being mapped/unmapped */
                phantom_isssd:1,
                volatile_empty:1,
                volatile_fault:1,
                all_reusable:1,
                blocked_access:1,
                set_cache_attr:1,
                object_slid:1,
                purgeable_queue_type:2,
                purgeable_queue_group:3,
                io_tracking:1,
                __object2_unused_bits:7;/* for expansion */
    
    uint8_t		scan_collisions;
    vm_tag_t	wire_tag;
    uint8_t		__object4_unused_bits[2];
    
#if CONFIG_PHANTOM_CACHE
    uint32_t		phantom_object_id;
#endif
#if CONFIG_IOSCHED || UPL_DEBUG
    //queue_head_t		uplq;		/* List of outstanding upls */
#endif
    
#ifdef	VM_PIP_DEBUG
    /*
     * Keep track of the stack traces for the first holders
     * of a "paging_in_progress" reference for this VM object.
     */
#define VM_PIP_DEBUG_STACK_FRAMES	25	/* depth of each stack trace */
#define VM_PIP_DEBUG_MAX_REFS		10	/* track that many references */
    struct __pip_backtrace {
        void *pip_retaddr[VM_PIP_DEBUG_STACK_FRAMES];
    } pip_holders[VM_PIP_DEBUG_MAX_REFS];
#endif	/* VM_PIP_DEBUG  */
    
    //queue_chain_t		objq;      /* object queue - currently used for purgable queues */
    
#if DEBUG
    void *purgeable_owner_bt[16];
    //task_t vo_purgeable_volatilizer; /* who made it volatile? */
    void *purgeable_volatilizer_bt[16];
#endif /* DEBUG */
};

#endif /* vm_object_h */
