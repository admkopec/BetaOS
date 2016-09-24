//
//  kern_return.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef kern_return_h
#define kern_return_h

#ifndef __ASSEMBLY__
typedef int kern_return_t;
#endif

#define KERN_SUCCESS                0

#define KERN_INVALID_ADDRESS		1
/* Specified address is not currently valid.
 */

#define KERN_PROTECTION_FAILURE		2
/* Specified memory is valid, but does not permit the
 * required forms of access.
 */

#define KERN_NO_SPACE               3
/* The address range specified is already in use, or
 * no address range of the size specified could be
 * found.
 */

#define KERN_INVALID_ARGUMENT		4
/* The function requested was not applicable to this
 * type of argument, or an argument is invalid
 */

#define KERN_FAILURE                5
/* The function could not be performed.  A catch-all.
 */

#define KERN_RESOURCE_SHORTAGE		6
/* A system resource could not be allocated to fulfill
 * this request.  This failure may not be permanent.
 */

#define KERN_NOT_RECEIVER           7
/* The task in question does not hold receive rights
 * for the port argument.
 */

#define KERN_NO_ACCESS              8
/* Bogus access restriction.
 */

#define KERN_MEMORY_FAILURE         9
/* During a page fault, the target address refers to a
 * memory object that has been destroyed.  This
 * failure is permanent.
 */

#define KERN_MEMORY_ERROR           10
/* During a page fault, the memory object indicated
 * that the data could not be returned.  This failure
 * may be temporary; future attempts to access this
 * same data may succeed, as defined by the memory
 * object.
 */

#define	KERN_ALREADY_IN_SET         11
/* The receive right is already a member of the portset.
 */

#define KERN_NOT_IN_SET             12
/* The receive right is not a member of a port set.
 */

#define KERN_NAME_EXISTS            13
/* The name already denotes a right in the task.
 */

#define KERN_ABORTED                14
/* The operation was aborted.  Ipc code will
 * catch this and reflect it as a message error.
 */

#define KERN_INVALID_NAME           15
/* The name doesn't denote a right in the task.
 */

#define	KERN_INVALID_TASK           16
/* Target task isn't an active task.
 */

#define KERN_INVALID_RIGHT          17
/* The name denotes a right, but not an appropriate right.
 */

#define KERN_INVALID_VALUE          18
/* A blatant range error.
 */

#define	KERN_UREFS_OVERFLOW         19
/* Operation would overflow limit on user-references.
 */

#define	KERN_INVALID_CAPABILITY		20
/* The supplied (port) capability is improper.
 */

#define KERN_RIGHT_EXISTS           21
/* The task already has send or receive rights
 * for the port under another name.
 */

#define	KERN_INVALID_HOST           22
/* Target host isn't actually a host.
 */

#define KERN_MEMORY_PRESENT         23
/* An attempt was made to supply "precious" data
 * for memory that is already present in a
 * memory object.
 */

#define KERN_MEMORY_DATA_MOVED		24
/* A page was requested of a memory manager via
 * memory_object_data_request for an object using
 * a MEMORY_OBJECT_COPY_CALL strategy, with the
 * VM_PROT_WANTS_COPY flag being used to specify
 * that the page desired is for a copy of the
 * object, and the memory manager has detected
 * the page was pushed into a copy of the object
 * while the kernel was walking the shadow chain
 * from the copy to the object. This error code
 * is delivered via memory_object_data_error
 * and is handled by the kernel (it forces the
 * kernel to restart the fault). It will not be
 * seen by users.
 */

#define KERN_MEMORY_RESTART_COPY	25
/* A strategic copy was attempted of an object
 * upon which a quicker copy is now possible.
 * The caller should retry the copy using
 * vm_object_copy_quickly. This error code
 * is seen only by the kernel.
 */

#define KERN_INVALID_PROCESSOR_SET	26
/* An argument applied to assert processor set privilege
 * was not a processor set control port.
 */

#define KERN_POLICY_LIMIT           27
/* The specified scheduling attributes exceed the thread's
 * limits.
 */

#define KERN_INVALID_POLICY         28
/* The specified scheduling policy is not currently
 * enabled for the processor set.
 */

#define KERN_INVALID_OBJECT         29
/* The external memory manager failed to initialize the
 * memory object.
 */

#define KERN_ALREADY_WAITING		30
/* A thread is attempting to wait for an event for which
 * there is already a waiting thread.
 */

#define KERN_DEFAULT_SET            31
/* An attempt was made to destroy the default processor
 * set.
 */

#define KERN_EXCEPTION_PROTECTED	32
/* An attempt was made to fetch an exception port that is
 * protected, or to abort a thread while processing a
 * protected exception.
 */

#define KERN_INVALID_LEDGER         33
/* A ledger was required but not supplied.
 */

#define KERN_INVALID_MEMORY_CONTROL	34
/* The port was not a memory cache control port.
 */

#define KERN_INVALID_SECURITY		35
/* An argument supplied to assert security privilege
 * was not a host security port.
 */

#define KERN_NOT_DEPRESSED          36
/* thread_depress_abort was called on a thread which
 * was not currently depressed.
 */

#define KERN_TERMINATED             37
/* Object has been terminated and is no longer available
 */

#define KERN_LOCK_SET_DESTROYED		38
/* Lock set has been destroyed and is no longer available.
 */

#define KERN_LOCK_UNSTABLE          39
/* The thread holding the lock terminated before releasing
 * the lock
 */

#define KERN_LOCK_OWNED             40
/* The lock is already owned by another thread
 */

#define KERN_LOCK_OWNED_SELF		41
/* The lock is already owned by the calling thread
 */

#define KERN_SEMAPHORE_DESTROYED	42
/* Semaphore has been destroyed and is no longer available.
 */

#define KERN_RPC_SERVER_TERMINATED	43
/* Return from RPC indicating the target server was
 * terminated before it successfully replied
 */

#define KERN_RPC_TERMINATE_ORPHAN	44
/* Terminate an orphaned activation.
 */

#define KERN_RPC_CONTINUE_ORPHAN	45
/* Allow an orphaned activation to continue executing.
 */

#define	KERN_NOT_SUPPORTED          46
/* Empty thread activation (No thread linked to it)
 */

#define	KERN_NODE_DOWN              47
/* Remote node down or inaccessible.
 */

#define KERN_NOT_WAITING            48
/* A signalled thread was not actually waiting. */

#define	KERN_OPERATION_TIMED_OUT    49
/* Some thread-oriented operation (semaphore_wait) timed out
 */

#define KERN_CODESIGN_ERROR         50
/* During a page fault, indicates that the page was rejected
 * as a result of a signature check.
 */

#define KERN_POLICY_STATIC          51
/* The requested property cannot be changed at this time.
 */

#define KERN_INSUFFICIENT_BUFFER_SIZE	52
/* The provided buffer is of insufficient size for the requested data.
 */

#define	KERN_RETURN_MAX             0x100
/* Maximum return value allowable
 */

#endif /* kern_return_h */
