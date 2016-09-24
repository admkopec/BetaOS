//
//  processor.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/28/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef processor_h
#define processor_h
#include <kernel/queue.h>

typedef struct processor *processor_t;

struct processor {
    queue_chain_t		processor_queue;/* idle/active queue link,
                                         * MUST remain the first element */
    int					state;			/* See below */
    bool                is_SMT;
    bool                is_recommended;
    struct thread
    *active_thread,	/* thread running on processor */
    *next_thread,	/* next thread when dispatched */
    *idle_thread;	/* this processor's idle thread. */
    
    //processor_set_t		processor_set;	/* assigned set */
    
    int					current_pri;	/* priority of current thread */
    //sched_mode_t		current_thmode;	/* sched mode of current thread */
    //sfi_class_id_t		current_sfi_class;	/* SFI class of current thread */
    int					cpu_id;			/* platform numeric id */
    
    //timer_call_data_t	quantum_timer;	/* timer for quantum expiration */
    uint64_t			quantum_end;	/* time when current quantum ends */
    uint64_t			last_dispatch;	/* time of last dispatch */
    
    uint64_t			deadline;		/* current deadline */
    bool                first_timeslice;                /* has the quantum expired since context switch */
    
#if defined(CONFIG_SCHED_TRADITIONAL) || defined(CONFIG_SCHED_MULTIQ)
    struct run_queue	runq;			/* runq for this processor */
#endif
    
#if defined(CONFIG_SCHED_TRADITIONAL)
    int					runq_bound_count; /* # of threads bound to this processor */
#endif
#if defined(CONFIG_SCHED_GRRR)
    struct grrr_run_queue	grrr_runq;      /* Group Ratio Round-Robin runq */
#endif
    
    processor_t			processor_primary;	/* pointer to primary processor for
                                             * secondary SMT processors, or a pointer
                                             * to ourselves for primaries or non-SMT */
    processor_t         processor_secondary;
    struct ipc_port *	processor_self;     /* port for operations */
    
    processor_t			processor_list;     /* all existing processors */
    //processor_data_t	processor_data;     /* per-processor data */
};


#endif /* processor_h */
