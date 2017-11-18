//
//  thread.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/28/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef thread_h
#define thread_h

#include <stdint.h>
#include <stdbool.h>
#include <i386/vm_types.h>
#include <kernel/processor.h>

struct thread {
    /*
     *	NOTE:	The runq field in the thread structure has an unusual
     *	locking protocol.  If its value is PROCESSOR_NULL, then it is
     *	locked by the thread_lock, but if its value is something else
     *	then it is locked by the associated run queue lock. It is
     *	set to PROCESSOR_NULL without holding the thread lock, but the
     *	transition from PROCESSOR_NULL to non-null must be done
     *	under the thread lock and the run queue lock.
     *
     *	New waitq APIs allow the 'links' and 'runq' fields to be
     *	anywhere in the thread structure.
     */
    /* Items examined often, modified infrequently */
    queue_chain_t	links;				/* run/wait queue links */
    processor_t		runq;				/* run queue assignment */
    event64_t		wait_event;			/* wait queue event */
    struct waitq	*waitq;
    /* Data updated during assert_wait/thread_wakeup */
#if __SMP__
    decl_simple_lock_data(,sched_lock)	/* scheduling lock (thread_lock()) */
    decl_simple_lock_data(,wake_lock)	/* for thread stop / wait (wake_lock()) */
#endif
    int                         options;    /* options set by thread itself */
#define TH_OPT_INTMASK          0x0003		/* interrupt / abort level */
#define TH_OPT_VMPRIV           0x0004		/* may allocate reserved memory */
#define TH_OPT_DTRACE           0x0008		/* executing under dtrace_probe */
#define TH_OPT_SYSTEM_CRITICAL	0x0010		/* Thread must always be allowed to run - even under heavy load */
#define TH_OPT_PROC_CPULIMIT	0x0020		/* Thread has a task-wide CPU limit applied to it */
#define TH_OPT_PRVT_CPULIMIT	0x0040		/* Thread has a thread-private CPU limit applied to it */
#define TH_OPT_IDLE_THREAD      0x0080		/* Thread is a per-processor idle thread */
#define TH_OPT_GLOBAL_FORCED_IDLE	0x0100	/* Thread performs forced idle for thermal control */
#define TH_OPT_SCHED_VM_GROUP	0x0200		/* Thread belongs to special scheduler VM group */
#define TH_OPT_HONOR_QLIMIT     0x0400		/* Thread will honor qlimit while sending msg, regardless of SEND_ALWAYS */
    
    bool                wake_active;        /* wake event on stop */
    int					at_safe_point;      /* thread_abort_safely allowed */
    ast_t				reason;             /* why we blocked */
    uint32_t 			quantum_remaining;
    wait_result_t 		wait_result;        /* outcome of wait -
                                             * may be examined by this thread
                                             * WITHOUT locking */
    thread_continue_t	continuation;       /* continue here next dispatch */
    void				*parameter;         /* continuation parameter */
    
    /* Data updated/used in thread_invoke */
    vm_offset_t     	kernel_stack;		/* current kernel stack */
    vm_offset_t			reserved_stack;		/* reserved kernel stack */
    
    /* Thread state: */
    int					state;
    /*
     *	Thread states [bits or'ed]
     */
#define TH_WAIT             0x01			/* queued for waiting */
#define TH_SUSP             0x02			/* stopped or requested to stop */
#define TH_RUN              0x04			/* running or on runq */
#define TH_UNINT            0x08			/* waiting uninteruptibly */
#define TH_TERMINATE		0x10			/* halted at termination */
#define TH_TERMINATE2		0x20			/* added to termination queue */
    
#define TH_IDLE             0x80			/* idling processor */
    
    /* Scheduling information */
    sched_mode_t			sched_mode;		/* scheduling mode */
    sched_mode_t			saved_mode;		/* saved mode during forced mode demotion */
    
    sfi_class_id_t			sfi_class;		/* SFI class (XXX Updated on CSW/QE/AST) */
    sfi_class_id_t			sfi_wait_class;	/* Currently in SFI wait for this class, protected by sfi_lock */
    
    uint32_t			sched_flags;		/* current flag bits */
    /* TH_SFLAG_FAIRSHARE_TRIPPED (unused)	0x0001 */
#define TH_SFLAG_FAILSAFE		0x0002		/* fail-safe has tripped */
#define TH_SFLAG_THROTTLED		0x0004		/* thread treated as background for scheduler decay purposes */
#define TH_SFLAG_DEMOTED_MASK      (TH_SFLAG_THROTTLE_DEMOTED | TH_SFLAG_FAILSAFE)	/* saved_mode contains previous sched_mode */
    
#define	TH_SFLAG_PROMOTED           0x0008		/* sched pri has been promoted */
#define TH_SFLAG_ABORT              0x0010		/* abort interruptible waits */
#define TH_SFLAG_ABORTSAFELY		0x0020		/* ... but only those at safe point */
#define TH_SFLAG_ABORTED_MASK		(TH_SFLAG_ABORT | TH_SFLAG_ABORTSAFELY)
#define	TH_SFLAG_DEPRESS            0x0040		/* normal depress yield */
#define TH_SFLAG_POLLDEPRESS		0x0080		/* polled depress yield */
#define TH_SFLAG_DEPRESSED_MASK		(TH_SFLAG_DEPRESS | TH_SFLAG_POLLDEPRESS)
#define TH_SFLAG_PRI_UPDATE         0x0100		/* Updating priority */
#define TH_SFLAG_EAGERPREEMPT		0x0200		/* Any preemption of this thread should be treated as if AST_URGENT applied */
#define TH_SFLAG_RW_PROMOTED		0x0400		/* sched pri has been promoted due to blocking with RW lock held */
#define TH_SFLAG_THROTTLE_DEMOTED	0x0800		/* throttled thread forced to timeshare mode (may be applied in addition to failsafe) */
#define TH_SFLAG_WAITQ_PROMOTED		0x1000		/* sched pri promoted from waitq wakeup (generally for IPC receive) */
#define TH_SFLAG_PROMOTED_MASK		(TH_SFLAG_PROMOTED | TH_SFLAG_RW_PROMOTED | TH_SFLAG_WAITQ_PROMOTED)
    
#define TH_SFLAG_RW_PROMOTED_BIT	(10)	/* 0x400 */
    
    int16_t                         sched_pri;              /* scheduled (current) priority */
    int16_t                         base_pri;               /* base priority */
    int16_t                         max_priority;           /* copy of max base priority */
    int16_t                         task_priority;          /* copy of task base priority */
    
#if defined(CONFIG_SCHED_GRRR)
#if 0
    uint16_t			grrr_deficit;		/* fixed point (1/1000th quantum) fractional deficit */
#endif
#endif
    
    int16_t				promotions;			/* level of promotion */
    int16_t				pending_promoter_index;
    uint32_t			ref_count;          /* number of references to me */
    void				*pending_promoter[2];
    
    uint32_t			rwlock_count;       /* Number of lck_rw_t locks held by thread */
    
#if ASSERT
    uint32_t			SHARE_COUNT, BG_COUNT; /* This thread's contribution to global sched counters (temporary debugging) */
#endif /* ASSERT */
    
    int                 importance;			/* task-relative importance */
    uint32_t            was_promoted_on_wakeup;
    
    /* Priority depression expiration */
    int                 depress_timer_active;
    timer_call_data_t	depress_timer;
    /* real-time parameters */
    struct {
        uint32_t		period;
        uint32_t		computation;
        uint32_t		constraint;
        bool            preemptible;
        uint64_t		deadline;
    } realtime;
    
    uint64_t			last_run_time;              /* time when thread was switched away from */
    uint64_t			last_made_runnable_time;	/* time when thread was unblocked or preempted */
    
#if defined(CONFIG_SCHED_MULTIQ)
    sched_group_t			sched_group;
#endif /* defined(CONFIG_SCHED_MULTIQ) */
    
    /* Data used during setrun/dispatch */
    timer_data_t		system_timer;		/* system mode timer */
    processor_t			bound_processor;	/* bound to a processor? */
    processor_t			last_processor;		/* processor last dispatched on */
    processor_t			chosen_processor;	/* Where we want to run this thread */
    
    /* Fail-safe computation since last unblock or qualifying yield */
    uint64_t			computation_metered;
    uint64_t			computation_epoch;
    uint64_t			safe_release;	/* when to release fail-safe */
    
    /* Call out from scheduler */
    void				(*sched_call)(
                                      int			type,
                                      thread_t	thread);
#if defined(CONFIG_SCHED_PROTO)
    uint32_t			runqueue_generation;	/* last time runqueue was drained */
#endif
    
    /* Statistics and timesharing calculations */
#if defined(CONFIG_SCHED_TIMESHARE_CORE)
    unsigned int		sched_stamp;	/* last scheduler tick */
    unsigned int		sched_usage;	/* timesharing cpu usage [sched] */
    unsigned int		pri_shift;		/* usage -> priority from pset */
    unsigned int		cpu_usage;		/* instrumented cpu usage [%cpu] */
    unsigned int		cpu_delta;		/* accumulated cpu_usage delta */
#endif /* CONFIG_SCHED_TIMESHARE_CORE */
    
    uint32_t			c_switch;		/* total context switches */
    uint32_t			p_switch;		/* total processor switches */
    uint32_t			ps_switch;		/* total pset switches */
    
    int                 mutex_count;    /* total count of locks held */
    /* Timing data structures */
    int					precise_user_kernel_time; /* precise user/kernel enabled for this thread */
    timer_data_t		user_timer;			/* user mode timer */
    uint64_t			user_timer_save;	/* saved user timer value */
    uint64_t			system_timer_save;	/* saved system timer value */
    uint64_t			vtimer_user_save;	/* saved values for vtimers */
    uint64_t			vtimer_prof_save;
    uint64_t			vtimer_rlim_save;
    
#if CONFIG_SCHED_SFI
    /* Timing for wait state */
    uint64_t		wait_sfi_begin_time;    /* start time for thread waiting in SFI */
#endif
    
    /* Timed wait expiration */
    timer_call_data_t	wait_timer;
    int                 wait_timer_active;
    bool                wait_timer_is_set;
    
    
    /*
     * Processor/cache affinity
     * - affinity_threads links task threads with the same affinity set
     */
    affinity_set_t			affinity_set;
    queue_chain_t			affinity_threads;
    
    /* Various bits of stashed state */
    union {
        struct {
            mach_msg_return_t	state;		/* receive state */
            mach_port_seqno_t	seqno;		/* seqno of recvd message */
            ipc_object_t		object;		/* object received on */
            mach_vm_address_t	msg_addr;	/* receive buffer pointer */
            mach_msg_size_t		msize;		/* max size for recvd msg */
            mach_msg_option_t	option;		/* options for receive */
            mach_port_name_t	receiver_name;	/* the receive port name */
            struct ipc_kmsg		*kmsg;		/* received message */
            mach_msg_continue_t	continuation;
        } receive;
        struct {
            struct semaphore	*waitsemaphore;  	/* semaphore ref */
            struct semaphore	*signalsemaphore;	/* semaphore ref */
            int					options;			/* semaphore options */
            kern_return_t		result;				/* primary result */
            mach_msg_continue_t continuation;
        } sema;
        struct {
            int					option;                     /* switch option */
            bool				reenable_workq_callback;	/* on entry, callbacks were suspended */
        } swtch;
        int						misc;                       /* catch-all for other state */
    } saved;
    
    /* Structure to save information about guard exception */
    struct {
        unsigned                        type;       /* EXC_GUARD reason/type */
        mach_exception_data_type_t		code;		/* Exception code */
        mach_exception_data_type_t		subcode;	/* Exception sub-code */
    } guard_exc_info;
    
    /* Kernel holds on this thread  */
    int16_t                                         suspend_count;
    /* User level suspensions */
    int16_t                                         user_stop_count;
    
    /* IPC data structures */
#if IMPORTANCE_INHERITANCE
    natural_t ith_assertions;               /* assertions pending drop */
#endif
    struct ipc_kmsg_queue ith_messages;		/* messages to reap */
    //mach_port_t ith_rpc_reply;              /* reply port for kernel RPCs */
    
    /* Ast/Halt data structures */
    vm_offset_t					recover;		/* page fault recover(copyin/out) */
    
    queue_chain_t				threads;		/* global list of all threads */
    
    /* Activation */
    queue_chain_t			task_threads;
    
    /* Task membership */
    struct task				*task;
    vm_map_t				map;
    
    decl_lck_mtx_data(,mutex)
    
    
    /* Pending thread ast(s) */
    ast_t					ast;
    
    /* Miscellaneous bits guarded by mutex */
    uint32_t
active:1,				/* Thread is active and has not been terminated */
started:1,				/* Thread has been started after creation */
static_param:1,			/* Disallow policy parameter changes */
inspection:1,			/* true when task is being inspected by crash reporter */
policy_reset:1,			/* Disallow policy parameter changes on terminating threads */
    :0;
    
    /* Ports associated with this thread */
    struct ipc_port			*ith_self;		/* not a right, doesn't hold ref */
    struct ipc_port			*ith_sself;		/* a send right */
    struct exception_action	*exc_actions;
    
#ifdef	MACH_BSD
    void					*uthread;
#endif
    
#if CONFIG_DTRACE
    uint32_t t_dtrace_flags;        /* DTrace thread states */
#define	TH_DTRACE_EXECSUCCESS	0x01
    uint32_t t_dtrace_predcache;    /* DTrace per thread predicate value hint */
    int64_t t_dtrace_tracing;       /* Thread time under dtrace_probe() */
    int64_t t_dtrace_vtime;
#endif
    
    clock_sec_t t_page_creation_time;
    uint32_t    t_page_creation_count;
    uint32_t    t_page_creation_throttled;
#if (DEVELOPMENT || DEBUG)
    uint64_t    t_page_creation_throttled_hard;
    uint64_t    t_page_creation_throttled_soft;
#endif /* DEVELOPMENT || DEBUG */
    
#define T_CHUD_MARKED           0x01          /* this thread is marked by CHUD */
#define T_IN_CHUD               0x02          /* this thread is already in a CHUD handler */
#define THREAD_PMC_FLAG         0x04          /* Bit in "t_chud" signifying PMC interest */
#define T_AST_CALLSTACK         0x08          /* Thread scheduled to dump a
* callstack on its next
* AST */
#define T_AST_NAME              0x10          /* Thread scheduled to dump
* its name on its next
* AST */
#define T_NAME_DONE             0x20          /* Thread has previously
* recorded its name */
#define T_KPC_ALLOC             0x40          /* Thread needs a kpc_buf */
    
    uint32_t t_chud;                          /* CHUD flags, used for Shark */
    uint32_t chud_c_switch;                   /* last dispatch detection */
    
#ifdef KPC
    /* accumulated performance counters for this thread */
    uint64_t *kpc_buf;
#endif
    
#ifdef KPERF
    /* count of how many times a thread has been sampled since it was last scheduled */
    uint64_t kperf_pet_cnt;
#endif
    
#if HYPERVISOR
    /* hypervisor virtual CPU object associated with this thread */
    void *hv_thread_target;
#endif /* HYPERVISOR */
    
    uint64_t thread_id;	/*system wide unique thread-id*/
    
    /* Statistics accumulated per-thread and aggregated per-task */
    uint32_t		syscalls_unix;
    uint32_t		syscalls_mach;
    ledger_t		t_ledger;
    ledger_t		t_threadledger;	/* per thread ledger */
    uint64_t 		cpu_time_last_qos;
#ifdef CONFIG_BANK
    ledger_t		t_bankledger;  		   /* ledger to charge someone */
    uint64_t		t_deduct_bank_ledger_time; /* cpu time to be deducted from bank ledger */
#endif
    
    /* policy is protected by the task lock */
    struct task_requested_policy     requested_policy;
    struct task_effective_policy     effective_policy;
    struct task_pended_policy        pended_policy;
    
    /* usynch override is protected by the task lock, eventually will be thread mutex */
    struct thread_qos_override {
        struct thread_qos_override	*override_next;
        uint32_t	override_contended_resource_count;
        int16_t		override_qos;
        int16_t		override_resource_type;
        user_addr_t	override_resource;
    } *overrides;
    
    int	iotier_override; /* atomic operations to set, cleared on ret to user */
    int                     saved_importance;               /* saved task-relative importance */
    io_stat_info_t  		thread_io_stats;                /* per-thread I/O statistics */
    
    
    uint32_t			thread_callout_interrupt_wakeups;
    uint32_t			thread_callout_platform_idle_wakeups;
    uint32_t			thread_timer_wakeups_bin_1;
    uint32_t			thread_timer_wakeups_bin_2;
    uint16_t			thread_tag;
    uint16_t			callout_woken_from_icontext:1,
callout_woken_from_platform_idle:1,
callout_woke_thread:1,
thread_bitfield_unused:13;
    
    mach_port_name_t		ith_voucher_name;
    ipc_voucher_t			ith_voucher;
#if CONFIG_IOSCHED
    void 				*decmp_upl;
#endif /* CONFIG_IOSCHED */
    
    /* work interval ID (if any) associated with the thread. Uses thread mutex */
    uint64_t		work_interval_id;
    
    /*** Machine-dependent state ***/
    struct machine_thread   machine;
};

typedef struct thread *thread_t;

#endif /* thread_h */
