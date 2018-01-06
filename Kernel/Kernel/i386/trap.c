//
//  trap.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/7/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include "trap.h"
#include "cpu_threads.h"
#include "thread_status.h"
#include "lapic.h"
#include "misc_protos.h"
#include "cpu_topology.h"
#include "eflags.h"
#include "pal.h"
#include "rtclock.h"
#include "machine_routines.h"
#include "vm_prot.h"
#include "pal.h"
#include "pio.h"

extern uint64_t col, line;
extern void IncommingInterrupt(int InterruptNumber);
extern void refresh_screen(void);

static void panic_trap(x86_saved_state64_t *regs, uint32_t pl);
static void set_recovery_ip(x86_saved_state64_t  *saved_state, vm_offset_t ip);

extern void switchTasks(void);

#ifdef DEBUG
void printf_state(x86_saved_state64_t *saved_state);
#endif

void
i386_exception(int exc, uint64_t code, uint64_t subcode) {
	uint64_t   codes[EXCEPTION_CODE_MAX];
    
	printf("i386_exception: exc=%d code=0x%llx subcode=0x%llx\n", exc, code, subcode);
	codes[0] = code;		/* new exception interface */
	codes[1] = subcode;
	//exception_triage(exc, codes, 2);
	/*NOTREACHED*/
}

/*
 * Handle interrupts:
 *  - local APIC interrupts (IPIs, timers, etc) are handled by the kernel,
 *  - device interrupts go to the platform expert.
 */
void
interrupt(x86_saved_state_t *state) {
    state = (x86_saved_state_t *)ml_static_ptovirt((vm_offset_t)state);
    uint64_t	rip;
    uint64_t	rsp;
    int         interrupt_num;
    bool        user_mode = false;
    int         ipl;
    int         cnum = cpu_number();
    cpu_data_t	*cdp = cpu_data_ptr[cnum];
    __unused int         itype = 0;
    
    if (is_saved_state64(state) == true) {
        x86_saved_state64_t	*state64;
        
        state64         = (x86_saved_state64_t *)ml_static_ptovirt((vm_offset_t)saved_state64(state));
        rip             = state64->isf.rip;
        rsp             = state64->isf.rsp;
        interrupt_num   = state64->isf.trapno;
        if(state64->isf.cs & 0x03)
            user_mode = true;
    } else {
        x86_saved_state32_t	*state32;
        
        state32         = (x86_saved_state32_t *)ml_static_ptovirt((vm_offset_t)saved_state32(state));
        if (state32->cs & 0x03)
            user_mode   = true;
        rip             = state32->eip;
        rsp             = state32->uesp;
        interrupt_num   = state32->trapno;
    }
//    printf("Interrupt_Num: 0x%x\n", interrupt_num);
    /*if (cpu_data_ptr[cnum]->lcpu.package->num_idle == topoParms.nLThreadsPerPackage)
        cpu_data_ptr[cnum]->cpu_hwIntpexits[interrupt_num]++;
    if (interrupt_num == (LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_INTERPROCESSOR_INTERRUPT))
        itype = 1;
    else if (interrupt_num == (LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_TIMER_INTERRUPT))
        itype = 2;
    else
        itype = 3;*/
    //KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, DBG_CODE(DBG_EXCP_INTR, 0) | DBG_FUNC_START, interrupt_num, (user_mode ? rip : VM_KERNEL_UNSLIDE(rip)), user_mode, itype, 0);
    //SCHED_STATS_INTERRUPT(current_processor());
    
#if CONFIG_TELEMETRY
    if (telemetry_needs_record) {
        telemetry_mark_curthread(user_mode);
    }
#endif
    
    ipl = get_preemption_level();
    
    /*
     * Handle local APIC interrupts
     * else call platform expert for devices.
     */
    
    if (!lapic_interrupt(interrupt_num, state)) {
        if (interrupt_num == LAPIC_DEFAULT_INTERRUPT_BASE) {
            refresh_screen();
            printf("");
            switchTasks();
        }
//        if (interrupt_num == 51) {
//            return_state = getFirstTask();
//        }
//        if (interrupt_num == 50) {
//            return_state = getNextTask(state);
//        }
        //Platform_incoming_interrupt(interrupt_num);
        if (interrupt_num >= LAPIC_DEFAULT_INTERRUPT_BASE && interrupt_num <= (LAPIC_DEFAULT_INTERRUPT_BASE + 0x0F)) {
            IncommingInterrupt(interrupt_num);
            if (interrupt_num < (LAPIC_DEFAULT_INTERRUPT_BASE + 8)) {
                outb(0x20,0x20);
            } else {
                outb(0x20, 0x20); outb(0xA0, 0x20);
            }
        }
    }
    
    if (__improbable(get_preemption_level() != ipl)) {
        panic("Preemption level altered by interrupt vector 0x%x: initial 0x%x, final: 0x%x\n", interrupt_num, ipl, get_preemption_level());
    }
    
    
    if (__improbable(cdp->cpu_nested_istack)) {
        cdp->cpu_nested_istack_events++;
    }
    else  {
        uint64_t ctime = absolute_time();
        __unused uint64_t int_latency = ctime - cdp->cpu_int_event_time;
        __unused uint64_t esdeadline, ehdeadline;
        /* Attempt to process deferred timers in the context of
         * this interrupt, unless interrupt time has already exceeded
         * TCOAL_ILAT_THRESHOLD.
         */
#define TCOAL_ILAT_THRESHOLD (30000ULL)
        
        /*if ((int_latency < TCOAL_ILAT_THRESHOLD) && interrupt_timer_coalescing_enabled) {
            esdeadline = cdp->rtclock_timer.queue.earliest_soft_deadline;
            ehdeadline = cdp->rtclock_timer.deadline;
            if ((ctime >= esdeadline) && (ctime < ehdeadline)) {
                interrupt_coalesced_timers++;
                //TCOAL_DEBUG(0x88880000 | DBG_FUNC_START, ctime, esdeadline, ehdeadline, interrupt_coalesced_timers, 0);
                rtclock_intr(state);
                //TCOAL_DEBUG(0x88880000 | DBG_FUNC_END, ctime, esdeadline, interrupt_coalesced_timers, 0, 0);
            } else {
                //TCOAL_DEBUG(0x77770000, ctime, cdp->rtclock_timer.queue.earliest_soft_deadline, cdp->rtclock_timer.deadline, interrupt_coalesced_timers, 0);
            }
        }*/
        
        //if (__improbable(ilat_assert && (int_latency > interrupt_latency_cap) && !machine_timeout_suspended())) {
        //    panic("Interrupt vector 0x%x exceeded interrupt latency threshold, 0x%llx absolute time delta, prior signals: 0x%x, current signals: 0x%x", interrupt_num, int_latency, cdp->cpu_prior_signals, cdp->cpu_signals);
        //}
        
        //if (__improbable(int_latency > cdp->cpu_max_observed_int_latency)) {
        //    cdp->cpu_max_observed_int_latency = int_latency;
        //    cdp->cpu_max_observed_int_latency_vector = interrupt_num;
        //}
    }
    
    /*
     * Having serviced the interrupt first, look at the interrupted stack depth.
     */
    if (!user_mode) {
        __unused uint64_t depth = cdp->cpu_kernel_stack + sizeof(struct x86_kernel_state) + sizeof(struct i386_exception_link *) - rsp;
        /*if (__improbable(depth > kernel_stack_depth_max)) {
            kernel_stack_depth_max = (vm_offset_t)depth;
            KERNEL_DEBUG_CONSTANT(MACHDBG_CODE(DBG_MACH_SCHED, MACH_STACK_DEPTH), (long) depth, (long) VM_KERNEL_UNSLIDE(rip), 0, 0, 0);
        }*/
    }
    
    //if (cnum == master_cpu)
    //    ml_entropy_collect();
    
    assert(ml_get_interrupts_enabled() == false);
}

static inline void
reset_dr7(void) {
	long dr7 = 0x400; /* magic dr7 reset value; 32 bit on i386, 64 bit on x86_64 */
	__asm__ volatile("mov %0,%%dr7" : : "r" (dr7));
}
/* temp */ typedef uint64_t user_addr_t;
void
kernel_trap(x86_saved_state_t	*state,
   __unused uintptr_t           *lo_spp) {
	x86_saved_state64_t	*saved_state;
	int                 code;
	user_addr_t         vaddr;
	int                 type;
//    vm_map_t            map = 0;    /* protected by T_PAGE_FAULT */
	kern_return_t		result = KERN_FAILURE;
	//thread_t            thread;
	//ast_t               *myast;
	bool               intr;
	vm_prot_t         prot;
    //struct recovery		*rp;
	vm_offset_t         kern_ip;
#if NCOPY_WINDOWS > 0
	int			fault_in_copy_window = -1;
#endif
    int         is_user = 0;
	int			trap_pl = get_preemption_level();
    
	//thread = current_thread();
    
	//if (__improbable(is_saved_state32(state)))
	//	panic("kernel_trap(%p) with 32-bit state", state);
    saved_state = (x86_saved_state64_t *)ml_static_ptovirt((vm_offset_t)saved_state64(state));
    
    // No read nor write to saved_state because of Page Faults.
    
	/* Record cpu where state was captured */
	saved_state->isf.cpu = cpu_number();
    
	vaddr   = (user_addr_t)saved_state->cr2;
	type    = saved_state->isf.trapno;
	code    = (int)(saved_state->isf.err & 0xffff);
	intr    = (saved_state->isf.rflags & EFL_IF) != 0;	/* state of ints at trap */
	kern_ip = (vm_offset_t)saved_state->isf.rip;
    
	//myast = ast_pending();
    
	//perfASTCallback astfn = perfASTHook;
	//if (__improbable(astfn != NULL)) {
		//if (*myast & AST_CHUD_ALL)
		//	astfn(AST_CHUD_ALL, myast);
    //} else
		//*myast &= ~AST_CHUD_ALL;
    
    
#if CONFIG_DTRACE
	/*
	 * Is there a DTrace hook?
	 */
	if (__improbable(tempDTraceTrapHook != NULL)) {
		if (tempDTraceTrapHook(type, state, lo_spp, 0) == KERN_SUCCESS) {
			/*
			 * If it succeeds, we are done...
			 */
			return;
		}
	}
#endif /* CONFIG_DTRACE */
    
	/*
	 * we come here with interrupts off as we don't want to recurse
	 * on preemption below.  but we do want to re-enable interrupts
	 * as soon we possibly can to hold latency down
	 */
	if (__improbable(T_PREEMPT == type)) {
        printf("T_PREEMPT!\n");
        goto debugger_entry;
        //ast_taken(AST_PREEMPTION, false);
		return;
	}
	
	if (T_PAGE_FAULT == type) {
        printf("Page Fault!\n");
		/*
		 * assume we're faulting in the kernel map
		 */
//        map = kernel_map;
        
		/*if (__probable(thread != THREAD_NULL && thread->map != kernel_map)) {
#if NCOPY_WINDOWS > 0
			vm_offset_t	copy_window_base;
			vm_offset_t	kvaddr;
			int		window_index;
            
			kvaddr = (vm_offset_t)vaddr;
            *
			 * must determine if fault occurred in
			 * the copy window while pre-emption is
			 * disabled for this processor so that
			 * we only need to look at the window
			 * associated with this processor
			 *
			copy_window_base = current_cpu_datap()->cpu_copywindow_base;
            
			if (kvaddr >= copy_window_base && kvaddr < (copy_window_base + (NBPDE * NCOPY_WINDOWS)) ) {
                
				window_index = (int)((kvaddr - copy_window_base) / NBPDE);
                
				if (thread->machine.copy_window[window_index].user_base != (user_addr_t)-1) {
                    
                    kvaddr -= (copy_window_base + (NBPDE * window_index));
                    vaddr = thread->machine.copy_window[window_index].user_base + kvaddr;
                    
					map = thread->map;
					fault_in_copy_window = window_index;
				}
				is_user = -1;
			}
#else
			if (__probable(vaddr < VM_MAX_USER_PAGE_ADDRESS)) {
				* fault occurred in userspace *
				//map = thread->map;
				is_user = -1;
                
				* Intercept a potential Supervisor Mode Execute
				 * Protection fault. These criteria identify
				 * both NX faults and SMEP faults, but both
				 * are fatal. We avoid checking PTEs (racy).
				 * (The VM could just redrive a SMEP fault, hence
				 * the intercept).
				 *
				if (__improbable((code == (T_PF_PROT | T_PF_EXECUTE)) && (pmap_smep_enabled) && (saved_state->isf.rip == vaddr))) {
					goto debugger_entry;
				}
                
				*
				 * Additionally check for SMAP faults...
				 * which are characterized by page-present and
				 * the AC bit unset (i.e. not from copyin/out path).
				 *
				if (__improbable(code & T_PF_PROT &&
                                 pmap_smap_enabled &&
                                 (saved_state->isf.rflags & EFL_AC) == 0)) {
					goto debugger_entry;
				}
                
				*
				 * If we're not sharing cr3 with the user
				 * and we faulted in copyio,
				 * then switch cr3 here and dismiss the fault.
				 *
				if (no_shared_cr3 &&
				    (thread->machine.specFlags&CopyIOActive) &&
				    map->pmap->pm_cr3 != get_cr3_base()) {
					pmap_assert(current_cpu_datap()->cpu_pmap_pcid_enabled == false);
					set_cr3_raw(map->pmap->pm_cr3);
					return;
				}
                
			}
#endif
		}*/
	}
    user_addr_t    kd_vaddr = is_user ? vaddr : VM_KERNEL_UNSLIDE(vaddr);
    printf("Faulty address = 0x%llx\n", kd_vaddr);
	//KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, (MACHDBG_CODE(DBG_MACH_EXCP_KTRAP_x86, type)) | DBG_FUNC_NONE, (unsigned)(kd_vaddr >> 32), (unsigned)kd_vaddr, is_user, VM_KERNEL_UNSLIDE(kern_ip), 0);
    
    
	(void) ml_set_interrupts_enabled(intr);
    
	switch (type) {
            
	    case T_NO_FPU:
            //fpnoextflt();
            printf("T_NO_FPU!\n");
            goto debugger_entry;
            return;
            
	    case T_FPU_FAULT:
            //fpextovrflt();
            printf("T_FPU_FAULT!\n");
            goto debugger_entry;
            return;
            
	    case T_FLOATING_POINT_ERROR:
            //fpexterrflt();
            printf("T_FLOATING_POINT_ERROR!\n");
            goto debugger_entry;
            return;
            
	    case T_SSE_FLOAT_ERROR:
            //fpSSEexterrflt();
            printf("T_SSE_FLOAT_ERROR!\n");
            goto debugger_entry;
            return;
 	    case T_DEBUG:
            printf("T_DEBUG!\n");
		    if ((saved_state->isf.rflags & EFL_TF) == 0 /*&& NO_WATCHPOINTS*/) {
			    /* We've somehow encountered a debug
			     * register match that does not belong
			     * to the kernel debugger.
			     * This isn't supposed to happen.
			     */
			    reset_dr7();
			    return;
		    }
		    goto debugger_entry;
#ifdef __x86_64__
	    case T_INT3:
            printf("T_INT3!\n");
            goto debugger_entry;
#endif
	    case T_PAGE_FAULT:
            printf("T_PAGE_FAULT\n");
            goto debugger_entry;
#if CONFIG_DTRACE
            //if (thread != THREAD_NULL && thread->options & TH_OPT_DTRACE) {	/* Executing under dtrace_probe? */
            //    if (dtrace_tally_fault(vaddr)) { /* Should a fault under dtrace be ignored? */
            //        /*
            //         * DTrace has "anticipated" the possibility of this fault, and has
            //         * established the suitable recovery state. Drop down now into the
            //         * recovery handling code in "case T_GENERAL_PROTECTION:".
            //         */
            //        goto FALL_THROUGH;
            //    }
            //}
#endif /* CONFIG_DTRACE */
            
            prot = VM_PROT_READ;
            
            if (code & T_PF_WRITE)
		        prot |= VM_PROT_WRITE;
            if (code & T_PF_EXECUTE)
		        prot |= VM_PROT_EXECUTE;
            
//            result = vm_fault(map, vm_map_trunc_page(vaddr, PAGE_MASK), prot, false, THREAD_UNINT, NULL, 0);
            
            if (result == KERN_SUCCESS) {
#if NCOPY_WINDOWS > 0
                /*if (fault_in_copy_window != -1) {
                    ml_set_interrupts_enabled(false);
                    copy_window_fault(thread, map,
                                      fault_in_copy_window);
                    (void) ml_set_interrupts_enabled(intr);
                }*/
#endif /* NCOPY_WINDOWS > 0 */
                return;
            }
            /*
             * fall through
             */
#if CONFIG_DTRACE
        FALL_THROUGH:
#endif /* CONFIG_DTRACE */
            
	    case T_GENERAL_PROTECTION:
            printf("T_GENERAL_PROTECTION!\n");
            goto debugger_entry;
            /*
             * If there is a failure recovery address
             * for this fault, go there.
             */
	        /*for (rp = recover_table; rp < recover_table_end; rp++) {
		        if (kern_ip == rp->fault_addr) {
			        set_recovery_ip(saved_state, rp->recover_addr);
                    return;
                }
            }*/
            
            /*
             * Check thread recovery address also.
             */
            /*if (thread != THREAD_NULL && thread->recover) {
                set_recovery_ip(saved_state, thread->recover);
                thread->recover = 0;
                return;
            }*/
            /*
             * Unanticipated page-fault errors in kernel
             * should not happen.
             *
             * fall through...
             */
	    default:
            /*
             * Exception 15 is reserved but some chips may generate it
             * spuriously. Seen at startup on AMD Athlon-64.
             */
	    	if (type == 15) {
                printf("kernel_trap() ignoring spurious trap 15\n");
                return;
            }
        debugger_entry:
            /* Ensure that the i386_kernel_state at the base of the
             * current thread's stack (if any) is synchronized with the
             * context at the moment of the trap, to facilitate
             * access through the debugger.
             */
            printf("");
            //sync_iss_to_iks(state);
/*#if KDP
            if (current_debugger != KDB_CUR_DB) {
                if (kdp_i386_trap(type, saved_state, result, (vm_offset_t)vaddr))
                    return;
            }
#endif*/
	}
	pal_cli();
	panic_trap(saved_state, trap_pl);
	/*
	 * NO RETURN
	 */
}

static void
__unused set_recovery_ip(x86_saved_state64_t  *saved_state, vm_offset_t ip) {
    saved_state->isf.rip = ip;
}

const char *	trap_type[] = {TRAP_NAMES};
unsigned        TRAP_TYPES  = sizeof(trap_type)/sizeof(trap_type[0]);

#if defined(__x86_64__) && DEBUG
void
printf_state_swift(x86_saved_state64_t state) {
    return printf_state(&state);
}

void
printf_state(x86_saved_state64_t *saved_state) {
    printf("current_cpu_datap() 0x%llx\n",  (uintptr_t)current_cpu_datap());
	printf("Current GS base MSR 0x%llx\n", rdmsr64(MSR_IA32_GS_BASE));
	printf("Kernel  GS base MSR 0x%llx\n", rdmsr64(MSR_IA32_KERNEL_GS_BASE));
    printf("state at 0x%llx:\n",            (uintptr_t) saved_state);
    
	printf("      rdi    0x%llx\n", saved_state->rdi);
	printf("      rsi    0x%llx\n", saved_state->rsi);
	printf("      rdx    0x%llx\n", saved_state->rdx);
	printf("      r10    0x%llx\n", saved_state->r10);
	printf("      r8     0x%llx\n", saved_state->r8);
	printf("      r9     0x%llx\n", saved_state->r9);
    
	printf("      cr2    0x%llx\n", saved_state->cr2);
	printf("real  cr2    0x%lx\n",  get_cr2());
	printf("      r15    0x%llx\n", saved_state->r15);
	printf("      r14    0x%llx\n", saved_state->r14);
	printf("      r13    0x%llx\n", saved_state->r13);
	printf("      r12    0x%llx\n", saved_state->r12);
	printf("      r11    0x%llx\n", saved_state->r11);
	printf("      rbp    0x%llx\n", saved_state->rbp);
	printf("      rbx    0x%llx\n", saved_state->rbx);
	printf("      rcx    0x%llx\n", saved_state->rcx);
	printf("      rax    0x%llx\n", saved_state->rax);
    
	printf("      gs     0x%x\n", saved_state->gs);
	printf("      fs     0x%x\n", saved_state->fs);
    
	printf("  isf.trapno 0x%x\n",   saved_state->isf.trapno);
	printf("  isf._pad   0x%x\n",   saved_state->isf._pad);
	printf("  isf.trapfn 0x%llx\n", saved_state->isf.trapfn);
	printf("  isf.err    0x%llx\n", saved_state->isf.err);
	printf("  isf.rip    0x%llx\n", saved_state->isf.rip);
	printf("  isf.cs     0x%llx\n", saved_state->isf.cs);
	printf("  isf.rflags 0x%llx\n", saved_state->isf.rflags);
	printf("  isf.rsp    0x%llx\n", saved_state->isf.rsp);
	printf("  isf.ss     0x%llx\n", saved_state->isf.ss);
}
#endif

#ifdef DEBUG
void
panic_idt64(x86_saved_state_t *rsp) {
    printf_state((x86_saved_state64_t *)(ml_static_ptovirt((vm_offset_t)saved_state64(rsp))));
    panic("panic_idt64");
}
#else
void
panic_idt64(__unused x86_saved_state_t *rsp) {
    panic("panic_idt64");
}
#endif

void
panic_64(x86_saved_state_t *sp, const char *msg, bool do_mca_dump) {
	/*
	 * Issue an I/O port read if one has been requested - this is an
	 * event logic analyzers can use as a trigger point.
	 */
	//panic_io_port_read();
    
	
	/*
	 * Break printf lock in case of recursion,
	 * and record originally faulted instruction address.
	 */
	//printf_break_lock();
    
	if (do_mca_dump) {
		/*
		 * Dump the contents of the machine check MSRs (if any).
		 */
		//mca_dump();
	}
    
	x86_saved_state64_t *regs = (x86_saved_state64_t *)(ml_static_ptovirt((vm_offset_t)saved_state64(sp)));
	panic("%s at 0x%016llx, registers:\n"
	      "CR0: 0x%016lx, CR2: 0x%016lx, CR3: 0x%016lx, CR4: 0x%016lx\n"
	      "RAX: 0x%016llx, RBX: 0x%016llx, RCX: 0x%016llx, RDX: 0x%016llx\n"
	      "RSP: 0x%016llx, RBP: 0x%016llx, RSI: 0x%016llx, RDI: 0x%016llx\n"
	      "R8:  0x%016llx, R9:  0x%016llx, R10: 0x%016llx, R11: 0x%016llx\n"
	      "R12: 0x%016llx, R13: 0x%016llx, R14: 0x%016llx, R15: 0x%016llx\n"
	      "RFL: 0x%016llx, RIP: 0x%016llx, CS:  0x%016llx, SS:  0x%016llx\n"
	      "Error code: 0x%016llx%s\n",
	      msg, regs->isf.rip,
          get_cr0(), get_cr2(), get_cr3_raw(), get_cr4(),
	      regs->rax, regs->rbx, regs->rcx, regs->rdx,
	      regs->isf.rsp, regs->rbp, regs->rsi, regs->rdi,
	      regs->r8,  regs->r9,  regs->r10, regs->r11,
	      regs->r12, regs->r13, regs->r14, regs->r15,
	      regs->isf.rflags, regs->isf.rip, regs->isf.cs & 0xFFFF,  regs->isf.ss & 0xFFFF,
	      regs->isf.err, /*virtualized ? " VMM" :*/ "");
}

void
panic_double_fault64(x86_saved_state_t *sp) {
	//(void)OSCompareAndSwap((UInt32) -1, (UInt32) cpu_number(), (volatile UInt32 *)&panic_double_fault_cpu);
	panic_64(sp, "Double fault", false);
    
}
void

panic_machine_check64(x86_saved_state_t *sp) {
	panic_64(sp, "Machine Check", true);
    
}

static void
panic_trap(x86_saved_state64_t *regs, __unused uint32_t pl) {
    regs = (x86_saved_state64_t *)ml_static_ptovirt((vm_offset_t)regs);
	const char	*trapname = "Unknown";
	pal_cr_t	cr0 = 0, cr2 = 0, cr3 = 0, cr4 = 0;
	bool        potential_smep_fault = false, potential_kernel_NX_fault = false;
	bool        potential_smap_fault = false;
    
	pal_get_control_registers( &cr0, &cr2, &cr3, &cr4 );
	assert(ml_get_interrupts_enabled() == false);
	current_cpu_datap()->cpu_fatal_trap_state = regs;
	/*
	 * Issue an I/O port read if one has been requested - this is an
	 * event logic analyzers can use as a trigger point.
	 */
	//panic_io_port_read();
    
	printf("panic trap number 0x%x, rip 0x%016llx\n",
            regs->isf.trapno, regs->isf.rip);
	printf("cr0 0x%016llx cr2 0x%016llx cr3 0x%016llx cr4 0x%016llx\n",
            cr0, cr2, cr3, cr4);
    
	if (regs->isf.trapno < TRAP_TYPES)
        trapname = trap_type[regs->isf.trapno];
    
	if ((regs->isf.trapno == T_PAGE_FAULT) && (regs->isf.err == (T_PF_PROT | T_PF_EXECUTE)) && (regs->isf.rip == regs->cr2)) {
		//if (pmap_smep_enabled && (regs->isf.rip < VM_MAX_USER_PAGE_ADDRESS)) {
		//	potential_smep_fault = true;
		//} else if (regs->isf.rip >= VM_MIN_KERNEL_AND_KEXT_ADDRESS) {
		//	potential_kernel_NX_fault = true;
		//}
	}// else if (pmap_smap_enabled && regs->isf.trapno == T_PAGE_FAULT && regs->isf.err & T_PF_PROT && regs->cr2 < VM_MAX_USER_PAGE_ADDRESS && regs->isf.rip >= VM_MIN_KERNEL_AND_KEXT_ADDRESS) {
     //	potential_smap_fault = true;
     //}
    
#undef panic
//    panic("Kernel trap at 0x%016llx, type %d=%s\n"
//          "Error code: 0x%016llx, Fault CPU: 0x%x\n",
//          regs->isf.rip, regs->isf.trapno, trapname,
//          regs->isf.err, regs->isf.cpu);
    
    panic("Kernel trap at 0x%016llx, type %d=%s, registers:\n"
          "CR0: 0x%016llx, CR2: 0x%016llx, CR3: 0x%016llx, CR4: 0x%016llx\n"
          "RAX: 0x%016llx, RBX: 0x%016llx, RCX: 0x%016llx, RDX: 0x%016llx\n"
          "RSP: 0x%016llx, RBP: 0x%016llx, RSI: 0x%016llx, RDI: 0x%016llx\n"
          "R8:  0x%016llx, R9:  0x%016llx, R10: 0x%016llx, R11: 0x%016llx\n"
          "R12: 0x%016llx, R13: 0x%016llx, R14: 0x%016llx, R15: 0x%016llx\n"
          "RFL: 0x%016llx, RIP: 0x%016llx, CS:  0x%016llx, SS:  0x%016llx\n"
          "Fault CR2: 0x%016llx, Error code: 0x%016llx, Fault CPU: 0x%x%s%s%s%s, PL: %d\n",
          regs->isf.rip, regs->isf.trapno, trapname,
          cr0, cr2, cr3, cr4,
          regs->rax, regs->rbx, regs->rcx, regs->rdx,
          regs->isf.rsp, regs->rbp, regs->rsi, regs->rdi,
          regs->r8,  regs->r9,  regs->r10, regs->r11,
          regs->r12, regs->r13, regs->r14, regs->r15,
          regs->isf.rflags, regs->isf.rip, regs->isf.cs & 0xFFFF,
          regs->isf.ss & 0xFFFF,regs->cr2, regs->isf.err, regs->isf.cpu,
          /*virtualized ? " VMM" : */"",
          potential_kernel_NX_fault ? " Kernel NX fault" : "",
          potential_smep_fault ? " SMEP/User NX fault" : "",
          potential_smap_fault ? " SMAP fault" : "", pl);
	/*
	 * This next statement is not executed,
	 * but it's needed to stop the compiler using tail call optimization
	 * for the panic call - which confuses the subsequent backtrace.
	 */
	cr0 = 0;
}



/*
 *	Trap from user mode.
 */
void
user_trap(x86_saved_state_t *saved_state) {
	int             exc;
	int             err;
	uint64_t        code;
	uint64_t     subcode;
	int             type;
	user_addr_t		vaddr;
	vm_prot_t		prot;
	//thread_t		thread = current_thread();
	//ast_t			*myast;
	kern_return_t	kret = 0;
	user_addr_t		rip;
	unsigned long 	dr6 = 0; /* 32 bit for i386, 64 bit for x86_64 */
    
	//assert((is_saved_state32(saved_state) && !thread_is_64bit(thread)) || (is_saved_state64(saved_state) &&  thread_is_64bit(thread)));
    
	if (is_saved_state64(saved_state)) {
        x86_saved_state64_t	*regs;
        
		regs = (x86_saved_state64_t*)(ml_static_ptovirt((vm_offset_t)saved_state64(saved_state)));
        
		/* Record cpu where state was captured */
		regs->isf.cpu = cpu_number();
        
		type = regs->isf.trapno;
		err  = (int)regs->isf.err & 0xffff;
		vaddr = (user_addr_t)regs->cr2;
		rip   = (user_addr_t)regs->isf.rip;
	} else {
		x86_saved_state32_t	*regs;
        
		regs = saved_state32(saved_state);
        
		/* Record cpu where state was captured */
		regs->cpu = cpu_number();
        
		type  = regs->trapno;
		err   = regs->err & 0xffff;
		vaddr = (user_addr_t)regs->cr2;
		rip   = (user_addr_t)regs->eip;
	}
    
	if (/*(*/type == T_DEBUG/*) && thread->machine.ids*/) {
		unsigned long clear = 0;
		/* Stash and clear this processor's DR6 value, in the event
		 * this was a debug register match
		 */
		__asm__ volatile ("mov %%db6, %0" : "=r" (dr6));
		__asm__ volatile ("mov %0, %%db6" : : "r" (clear));
	}
    
	pal_sti();
    
	//KERNEL_DEBUG_CONSTANT_IST(KDEBUG_TRACE, (MACHDBG_CODE(DBG_MACH_EXCP_UTRAP_x86, type)) | DBG_FUNC_NONE, (unsigned)(vaddr>>32), (unsigned)vaddr, (unsigned)(rip>>32), (unsigned)rip, 0);
    
	code    = 0;
	subcode = 0;
	exc     = 0;
    
#if DEBUG
    printf("user_trap(0x%08llx) type=%d vaddr=0x%016llx\n", (uintptr_t)saved_state, type, vaddr);
#endif
    
	//perfASTCallback astfn = perfASTHook;
	//if (__improbable(astfn != NULL)) {
	//	myast = ast_pending();
	//	if (*myast & AST_CHUD_ALL) {
	//		astfn(AST_CHUD_ALL, myast);
	//	}
	//}
    
	/* Is there a hook? */
	//perfCallback fn = perfTrapHook;
	//if (__improbable(fn != NULL)) {
	//	if (fn(type, saved_state, 0, 0) == KERN_SUCCESS)
	//		return;	/* If it succeeds, we are done... */
	//}
    
	/*
	 * DTrace does not consume all user traps, only INT_3's for now.
	 * Avoid needlessly calling tempDTraceTrapHook here, and let the
	 * INT_3 case handle them.
	 */
	//DEBUG_KPRINT_SYSCALL_MASK(1, "user_trap: type=0x%x(%s) err=0x%x cr2=%p rip=%p\n", type, trap_type[type], err, (void *)(long) vaddr, (void *)(long) rip);
	
	switch (type) {
            
	    case T_DIVIDE_ERROR:
            exc  = EXC_ARITHMETIC;
            code = EXC_I386_DIV;
            break;
            
	    case T_DEBUG: {
			//pcb_t	pcb;
			/*
			 * Update the PCB with this processor's DR6 value
			 * in the event this was a debug register match.
			 */
			//pcb = THREAD_TO_PCB(thread);
			//if (pcb->ids) {
				/*
				 * We can get and set the status register
				 * in 32-bit mode even on a 64-bit thread
				 * because the high order bits are not
				 * used on x86_64
				 */
			//	if (thread_is_64bit(thread)) {
			//		x86_debug_state64_t *ids = pcb->ids;
			//		ids->dr6 = dr6;
			//	} else { /* 32 bit thread */
			//		x86_debug_state32_t *ids = pcb->ids;
			//		ids->dr6 = (uint32_t) dr6;
			//	}
			//}
			exc  = EXC_BREAKPOINT;
            code = EXC_I386_SGL;
			break;
		}
	    case T_INT3:
#if CONFIG_DTRACE
            //if (dtrace_user_probe(saved_state) == KERN_SUCCESS)
            //    return; /* If it succeeds, we are done... */
#endif
            exc  = EXC_BREAKPOINT;
            code = EXC_I386_BPT;
            break;
            
	    case T_OVERFLOW:
            exc  = EXC_ARITHMETIC;
            code = EXC_I386_INTO;
            break;
            
	    case T_OUT_OF_BOUNDS:
            exc  = EXC_SOFTWARE;
            code = EXC_I386_BOUND;
            break;
            
	    case T_INVALID_OPCODE:
            exc  = EXC_BAD_INSTRUCTION;
            code = EXC_I386_INVOP;
            break;
            
	    case T_NO_FPU:
            //fpnoextflt();
            printf("T_NO_FPU! User\n");
            for (; ;) { }
            return;
            
	    case T_FPU_FAULT:
            //fpextovrflt(); /* Propagates exception directly, doesn't return */
            printf("T_FPU_FAULT! User\n");
            for (; ;) { }
            return;
            
	    case T_INVALID_TSS:	/* invalid TSS == iret with NT flag set */
            exc     = EXC_BAD_INSTRUCTION;
            code    = EXC_I386_INVTSSFLT;
            subcode = err;
            break;
            
	    case T_SEGMENT_NOT_PRESENT:
            exc     = EXC_BAD_INSTRUCTION;
            code    = EXC_I386_SEGNPFLT;
            subcode = err;
            break;
            
	    case T_STACK_FAULT:
            exc     = EXC_BAD_INSTRUCTION;
            code    = EXC_I386_STKFLT;
            subcode = err;
            break;
            
	    case T_GENERAL_PROTECTION:
            /*
             * There's a wide range of circumstances which generate this
             * class of exception. From user-space, many involve bad
             * addresses (such as a non-canonical 64-bit address).
             * So we map this to EXC_BAD_ACCESS (and thereby SIGSEGV).
             * The trouble is cr2 doesn't contain the faulting address;
             * we'd need to decode the faulting instruction to really
             * determine this. We'll leave that to debuggers.
             * However, attempted execution of privileged instructions
             * (e.g. cli) also generate GP faults and so we map these to
             * to EXC_BAD_ACCESS (and thence SIGSEGV) also - rather than
             * EXC_BAD_INSTRUCTION which is more accurate. We just can't
             * win!
             */
            exc     = EXC_BAD_ACCESS;
            code    = EXC_I386_GPFLT;
            subcode = err;
            break;
            
	    case T_PAGE_FAULT: {
		    prot = VM_PROT_READ;
            
            if (err & T_PF_WRITE)
		        prot |= VM_PROT_WRITE;
            if (__improbable(err & T_PF_EXECUTE))
		        prot |= VM_PROT_EXECUTE;
            //kret = vm_fault(thread->map, vm_map_trunc_page(vaddr, PAGE_MASK), prot, false, THREAD_ABORTSAFE, NULL, 0);
            
            if (__probable((kret == KERN_SUCCESS) || (kret == KERN_ABORTED))) {
                //thread_exception_return();
                /*NOTREACHED*/
            }
            
	        //user_page_fault_continue(kret);
	    }	/* NOTREACHED */
            break;
            
	    case T_SSE_FLOAT_ERROR:
            //fpSSEexterrflt(); /* Propagates exception directly, doesn't return */
            return;
            
            
	    case T_FLOATING_POINT_ERROR:
            //fpexterrflt(); /* Propagates exception directly, doesn't return */
            return;
            
	    case T_DTRACE_RET:
#if CONFIG_DTRACE
            //if (dtrace_user_probe(saved_state) == KERN_SUCCESS)
            //    return; /* If it succeeds, we are done... */
#endif
            /*
             * If we get an INT 0x7f when we do not expect to,
             * treat it as an illegal instruction
             */
            exc  = EXC_BAD_INSTRUCTION;
            code = EXC_I386_INVOP;
            break;
            
	    default:
            panic("Unexpected user trap, type %d", type);
            return;
	}
	/* Note: Codepaths that directly return from user_trap() have pending
	 * ASTs processed in locore
	 */
	i386_exception(exc, code, subcode);
	/* NOTREACHED */
}
