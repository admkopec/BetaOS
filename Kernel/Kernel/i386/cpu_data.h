//
//  cpu_data.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef cpu_data_h
#define cpu_data_h

#include <stdint.h>
#include <assert.h>
#include <kernel/queue.h>
#include <i386/pal.h>
#include <i386/tss.h>
#include <i386/seg.h>
#include <i386/thread_status.h>
#include <i386/cpu_topology.h>
//#include <kernel/thread.h>        Don't forget to implement!
//#include <kernel/processor.h>

/*
 * Data structures referenced (anonymously) from per-cpu data:
 */
struct cpu_cons_buffer;
struct cpu_desc_table;
struct mca_state;
struct prngContext;

/*
 * Data structures embedded in per-cpu data:
 */
typedef struct rtclock_timer {
    mpqueue_head_t		queue;
    uint64_t		deadline;
    uint64_t		when_set;
    bool            has_expired;
} rtclock_timer_t;


typedef struct {
    struct x86_64_tss	*cdi_ktss;
    struct __attribute__((packed)) {
        uint16_t size;
        void *ptr;
    } cdi_gdt, cdi_idt;
    struct fake_descriptor	*cdi_ldt;
    vm_offset_t		cdi_sstk;
} cpu_desc_index_t;
#ifndef TASK_MAP_DEFINED
typedef enum {
    TASK_MAP_32BIT,			/* 32-bit user, compatibility mode */
    TASK_MAP_64BIT,			/* 64-bit user thread, shared space */
} task_map_t;

typedef	uint16_t	pcid_t;
typedef	uint8_t		pcid_ref_t;
#define TASK_MAP_DEFINED
#endif

/*
 * This structure is used on entry into the (uber-)kernel on syscall from
 * a 64-bit user. It contains the address of the machine state save area
 * for the current thread and a temporary place to save the user's rsp
 * before loading this address into rsp.
 */
typedef struct {
    uint64_t	cu_isf;		/* thread->pcb->iss.isf */
    uint64_t	cu_tmp;		/* temporary scratch */
    uint64_t	cu_user_gs_base;
} cpu_uber_t;

#define CPU_RTIME_BINS (12)
#define CPU_ITIME_BINS (CPU_RTIME_BINS)

/* Per-CPU data */

typedef struct cpu_data
{
    //struct pal_cpu_data	cpu_pal_data;		/* PAL-specific data */
    #define				cpu_pd cpu_pal_data	/* convenience alias */
    struct cpu_data		*cpu_this;          /* pointer to myself */
    //thread_t            cpu_active_thread;
    //thread_t            cpu_nthread;
    volatile int		cpu_preemption_level;
    int                 cpu_number;         /* Logical CPU */
    void                *cpu_int_state;		/* interrupt state */
    vm_offset_t         cpu_active_stack;	/* kernel stack base */
    vm_offset_t         cpu_kernel_stack;	/* kernel stack top */
    vm_offset_t         cpu_int_stack_top;
    int                 cpu_interrupt_level;
    int                 cpu_phys_number;	/* Physical CPU */
    //cpu_id_t            cpu_id;             /* Platform Expert */
    volatile int		cpu_signals;		/* IPI events */
    volatile int		cpu_prior_signals;	/* Last set of events,
                                             * debugging
                                             */
    ast_t               cpu_pending_ast;
    volatile int		cpu_running;
    bool                cpu_fixed_pmcs_enabled;
    rtclock_timer_t		rtclock_timer;
    volatile uint64_t	cpu_active_cr3 __attribute((aligned(64)));
    union {
        volatile uint32_t cpu_tlb_invalid;
        struct {
            volatile uint16_t cpu_tlb_invalid_local;
            volatile uint16_t cpu_tlb_invalid_global;
        };
    };
    volatile task_map_t	cpu_task_map;
    volatile uint64_t	cpu_task_cr3;
    uint64_t            cpu_kernel_cr3;
    cpu_uber_t          cpu_uber;
    void                *cpu_chud;
    void                *cpu_console_buf;
    struct x86_lcpu		lcpu;
    //struct processor	*cpu_processor;     // I need it now! 
#if NCOPY_WINDOWS > 0
    struct cpu_pmap		*cpu_pmap;
#endif
    struct cpu_desc_table	*cpu_desc_tablep;
    struct fake_descriptor	*cpu_ldtp;
    cpu_desc_index_t        cpu_desc_index;
    int                     cpu_ldt;
#if NCOPY_WINDOWS > 0
    vm_offset_t		cpu_copywindow_base;
    uint64_t		*cpu_copywindow_pdp;
    
    vm_offset_t		cpu_physwindow_base;
    uint64_t		*cpu_physwindow_ptep;
#endif
    
#define HWINTCNT_SIZE 256
    uint32_t            cpu_hwIntCnt[HWINTCNT_SIZE];	/* Interrupt counts */
    uint64_t            cpu_hwIntpexits[HWINTCNT_SIZE];
    uint64_t            cpu_hwIntcexits[HWINTCNT_SIZE];
    uint64_t            cpu_dr7;                        /* debug control register */
    uint64_t            cpu_int_event_time;             /* intr entry/exit time */
    pal_rtc_nanotime_t	*cpu_nanotime;                  /* Nanotime info */
#if KPC
    /* double-buffered performance counter data */
    uint64_t                *cpu_kpc_buf[2];
    /* PMC shadow and reload value buffers */
    uint64_t                *cpu_kpc_shadow;
    uint64_t                *cpu_kpc_reload;
#endif
    uint32_t                cpu_pmap_pcid_enabled;
    pcid_t                  cpu_active_pcid;
    pcid_t                  cpu_last_pcid;
    volatile pcid_ref_t     *cpu_pmap_pcid_coherentp;
    volatile pcid_ref_t     *cpu_pmap_pcid_coherentp_kernel;
#define	PMAP_PCID_MAX_PCID      (0x1000)
    pcid_t			cpu_pcid_free_hint;
    pcid_ref_t		cpu_pcid_refcounts[PMAP_PCID_MAX_PCID];
    pmap_t			cpu_pcid_last_pmap_dispatched[PMAP_PCID_MAX_PCID];
#ifdef	PCID_STATS
    uint64_t		cpu_pmap_pcid_flushes;
    uint64_t		cpu_pmap_pcid_preserves;
#endif
    uint64_t		cpu_aperf;
    uint64_t		cpu_mperf;
    uint64_t		cpu_c3res;
    uint64_t		cpu_c6res;
    uint64_t		cpu_c7res;
    uint64_t		cpu_itime_total;
    uint64_t		cpu_rtime_total;
    uint64_t		cpu_ixtime;
    uint64_t        cpu_idle_exits;
    uint64_t		cpu_rtimes[CPU_RTIME_BINS];
    uint64_t		cpu_itimes[CPU_ITIME_BINS];
    uint64_t		cpu_cur_insns;
    uint64_t		cpu_cur_ucc;
    uint64_t		cpu_cur_urc;
    uint64_t		cpu_gpmcs[4];
    uint64_t        cpu_max_observed_int_latency;
    int             cpu_max_observed_int_latency_vector;
    volatile bool	cpu_NMI_acknowledged;
    uint64_t		debugger_entry_time;
    uint64_t		debugger_ipi_time;
    /* A separate nested interrupt stack flag, to account
     * for non-nested interrupts arriving while on the interrupt stack
     * Currently only occurs when AICPM enables interrupts on the
     * interrupt stack during processor offlining.
     */
    uint32_t            cpu_nested_istack;
    uint32_t            cpu_nested_istack_events;
    x86_saved_state64_t	*cpu_fatal_trap_state;
    x86_saved_state64_t	*cpu_post_fatal_trap_state;
#if CONFIG_VMX
    vmx_cpu_t           cpu_vmx;		/* wonderful world of virtualization */
#endif
#if CONFIG_MCA
    struct mca_state	*cpu_mca_state;		/* State at MC fault */
#endif
    struct prngContext	*cpu_prng;		/* PRNG's context */
    int			cpu_type;
    int			cpu_subtype;
    int			cpu_threadtype;
    bool		cpu_iflag;
    bool		cpu_boot_complete;
    int			cpu_hibernate;
} cpu_data_t;

extern cpu_data_t	*cpu_data_ptr[];

/* Macro to generate inline bodies to retrieve per-cpu data fields. */
#if defined(__clang__)
#define GS_RELATIVE volatile __attribute__((address_space(256)))
#ifndef offsetof
#define offsetof(TYPE,MEMBER) __builtin_offsetof(TYPE,MEMBER)
#endif

#define CPU_DATA_GET(member,type)										\
cpu_data_t GS_RELATIVE *cpu_data =                                      \
(cpu_data_t GS_RELATIVE *)0UL;                                          \
type ret;                                                               \
ret = cpu_data->member;                                                 \
return ret;

#define CPU_DATA_GET_INDEX(member,index,type)							\
cpu_data_t GS_RELATIVE *cpu_data =                                      \
(cpu_data_t GS_RELATIVE *)0UL;                                          \
type ret;                                                               \
ret = cpu_data->member[index];                                          \
return ret;

#define CPU_DATA_SET(member,value)										\
cpu_data_t GS_RELATIVE *cpu_data =                                      \
(cpu_data_t GS_RELATIVE *)0UL;                                          \
cpu_data->member = value;

#define CPU_DATA_XCHG(member,value,type)								\
cpu_data_t GS_RELATIVE *cpu_data =                                      \
(cpu_data_t GS_RELATIVE *)0UL;                                          \
type ret;                                                               \
ret = cpu_data->member;                                                 \
cpu_data->member = value;                                               \
return ret;

#endif/* defined(__clang__) */

/*
 * Everyone within the osfmk part of the kernel can use the fast
 * inline versions of these routines.  Everyone outside, must call
 * the real thing,
 */
/*static inline thread_t
get_active_thread(void)
{
    CPU_DATA_GET(cpu_active_thread,thread_t)
}*/
#define current_thread_fast()		get_active_thread()
#define current_thread()            current_thread_fast()

#define cpu_mode_is64bit()          true

// <Temporary>
extern void kernel_preempt_check(void);
// </Temporary>

static inline int get_preemption_level(void) {
    CPU_DATA_GET(cpu_preemption_level,int)
}

static inline int get_interrupt_level(void) {
    CPU_DATA_GET(cpu_interrupt_level,int)
}
static inline int get_cpu_number(void) {
    CPU_DATA_GET(cpu_number,int)
}
static inline int get_cpu_phys_number(void) {
    CPU_DATA_GET(cpu_phys_number,int)
}


static inline void disable_preemption(void) {
//#if defined(__clang__)
    cpu_data_t GS_RELATIVE *cpu_data = (cpu_data_t GS_RELATIVE *)0UL;
    cpu_data->cpu_preemption_level++;
//#endif
}

static inline void enable_preemption(void) {
    assert(get_preemption_level() > 0);
    
//#if defined(__clang__)
    cpu_data_t GS_RELATIVE *cpu_data = (cpu_data_t GS_RELATIVE *)0UL;
    if (0 == --cpu_data->cpu_preemption_level)
        kernel_preempt_check();
//#endif
}

static inline void enable_preemption_no_check(void) {
    assert(get_preemption_level() > 0);
    
#if defined(__clang__)
    cpu_data_t GS_RELATIVE *cpu_data = (cpu_data_t GS_RELATIVE *)0UL;
    cpu_data->cpu_preemption_level--;
#endif
}

static inline void mp_disable_preemption(void) {
    disable_preemption();
}

static inline void mp_enable_preemption(void) {
    enable_preemption();
}

static inline void mp_enable_preemption_no_check(void) {
    enable_preemption_no_check();
}

static inline cpu_data_t* current_cpu_datap(void) {
    CPU_DATA_GET(cpu_this, cpu_data_t *);
}

static inline cpu_data_t* cpu_datap(int cpu) {
    return cpu_data_ptr[cpu];
}

extern cpu_data_t *cpu_data_alloc(bool is_boot_cpu);
extern void cpu_data_realloc(void);


#endif /* cpu_data_h */
