//
//  vm_prot.h
//  Kernel
//
//  Created by Adam Kopeć on 2/18/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef vm_prot_h
#define vm_prot_h

typedef int vm_prot_t;

/*
 *	Protection values, defined as bits within the vm_prot_t type
 */

#define	VM_PROT_NONE	((vm_prot_t) 0x00)

#define VM_PROT_READ	((vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((vm_prot_t) 0x04)	/* execute permission */

/*
 *	The default protection for newly-created virtual memory
 */

#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE)

/*
 *	The maximum privileges possible, for parameter checking.
 */

#define VM_PROT_ALL	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

/*
 *	An invalid protection value.
 *	Used only by memory_object_lock_request to indicate no change
 *	to page locks.  Using -1 here is a bad idea because it
 *	looks like VM_PROT_ALL and then some.
 */

#define VM_PROT_NO_CHANGE	((vm_prot_t) 0x08)

/*
 *      When a caller finds that he cannot obtain write permission on a
 *      mapped entry, the following flag can be used.  The entry will
 *      be made "needs copy" effectively copying the object (using COW),
 *      and write permission will be added to the maximum protections
 *      for the associated entry.
 */

#define VM_PROT_COPY            ((vm_prot_t) 0x10)


/*
 *	Another invalid protection value.
 *	Used only by memory_object_data_request upon an object
 *	which has specified a copy_call copy strategy. It is used
 *	when the kernel wants a page belonging to a copy of the
 *	object, and is only asking the object as a result of
 *	following a shadow chain. This solves the race between pages
 *	being pushed up by the memory manager and the kernel
 *	walking down the shadow chain.
 */

#define VM_PROT_WANTS_COPY	((vm_prot_t) 0x10)

#ifdef PRIVATE
/*
 *	The caller wants this memory region treated as if it had a valid
 *	code signature.
 */

#define VM_PROT_TRUSTED		((vm_prot_t) 0x20)
#endif /* PRIVATE */

/*
 * 	Another invalid protection value.
 *	Indicates that the other protection bits are to be applied as a mask
 *	against the actual protection bits of the map entry.
 */
#define VM_PROT_IS_MASK		((vm_prot_t) 0x40)

#endif /* vm_prot_h */
