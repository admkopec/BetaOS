//
//  idt64.s
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <i386/asm.h>
#include <i386/thread_status.h>
#include <i386/pmap.h>
#include <i386/seg.h>
#include <i386/trap.h>
#include <i386/proc_reg.h>
#include <i386/vm_param.h>
#include <i386/eflags.h>
#include <i386/syscall.h>

/*
 * Handlers:
 */
#define	HNDL_ALLINTRS		EXT(hndl_allintrs)
#define	HNDL_ALLTRAPS		EXT(hndl_alltraps)
#define	HNDL_SYSENTER		EXT(hndl_sysenter)
#define	HNDL_SYSCALL		EXT(hndl_syscall)
#define	HNDL_UNIX_SCALL		EXT(hndl_unix_scall)
#define	HNDL_MACH_SCALL		EXT(hndl_mach_scall)
#define	HNDL_MDEP_SCALL		EXT(hndl_mdep_scall)
#define	HNDL_DOUBLE_FAULT	EXT(hndl_double_fault)
#define	HNDL_MACHINE_CHECK	EXT(hndl_machine_check)


#define PUSH_FUNCTION(func) 			 \
	sub	$8, %rsp                        ;\
	push	%rax                        ;\
	leaq	func(%rip), %rax            ;\
	movq	%rax, 8(%rsp)               ;\
	pop	%rax

/* The wrapper for all non-special traps/interrupts */
/* Everything up to PUSH_FUNCTION is just to output 
 * the interrupt number out to the postcode display
 */
#define IDT_ENTRY_WRAPPER(n, f)			 \
	PUSH_FUNCTION(f)                    ;\
	pushq	$(n)                        ;\
	jmp L_dispatch

/* A trap that comes with an error code already on the stack */
#define TRAP_ERR(n, f)				 \
	Entry(f)                        ;\
	IDT_ENTRY_WRAPPER(n, HNDL_ALLTRAPS)

/* A normal trap */
#define TRAP(n, f)                   \
	Entry(f)                        ;\
	pushq	$0          			;\
	IDT_ENTRY_WRAPPER(n, HNDL_ALLTRAPS)

#define USER_TRAP TRAP

/* An interrupt */
#define INTERRUPT(n)			 	 \
	Entry(_intr_ ## n)              ;\
	pushq	$0          			;\
	IDT_ENTRY_WRAPPER(n, HNDL_ALLINTRS)

/* A trap with a special-case handler, hence we don't need to define anything */
#define TRAP_SPC(n, f)
#define TRAP_IST1(n, f)
#define TRAP_IST2(n, f)
#define USER_TRAP_SPC(n, f)

/* Generate all the stubs */
//#include "idt_table.h"

/* Temporary defines, maybe not so temporary after all */
#define ISF64_TRAPNO        0x0     // offsetof(x86_64_intr_stack_frame_t, trapno)
#define ISF64_TRAPFN        0x8     // offsetof(x86_64_intr_stack_frame_t, trapfn)
#define ISF64_ERR           0x10    // offsetof(x86_64_intr_stack_frame_t. err)
#define ISF64_RIP           0x18    // offsetof(x86_64_intr_stack_frame_t. rip)
#define ISF64_CS            0x20    // offsetof(x86_64_intr_stack_frame_t, cs)
#define ISF64_RFLAGS        0x28    // offsetof(x86_64_intr_stack_frame_t, rflags)
#define ISF64_RSP           0x30    // offsetof(x86_64_intr_stack_frame_t, rsp)
#define ISF64_SS            0x38    // offsetof(x86_64_intr_stack_frame_t, ss)

#define R64_RDI             0x10    // offsetof(x86_saved_state_t, ss_64.rdi)
#define R64_RSI             0x18    // offsetof(x86_saved_state_t, ss_64.rsi)
#define R64_RDX             0x20    // offsetof(x86_saved_state_t, ss_64.rdx)
#define R64_R10             0x28    // offsetof(x86_saved_state_t, ss_64.r10)
#define R64_R8              0x30    // offsetof(x86_saved_state_t, ss_64.r8)
#define R64_R9              0x38    // offsetof(x86_saved_state_t, ss_64.r9)
#define R64_CR2             0x40    // offsetof(x86_saved_state_t, ss_64.cr2)
#define R64_R15             0x48    // offsetof(x86_saved_state_t, ss_64.r15)
#define R64_R14             0x50    // offsetof(x86_saved_state_t, ss_64.r14)
#define R64_R13             0x58    // offsetof(x86_saved_state_t, ss_64.r13)
#define R64_R12             0x60    // offsetof(x86_saved_state_t, ss_64.r12)
#define R64_R11             0x68    // offsetof(x86_saved_state_t, ss_64.r11)
#define R64_RBP             0x70    // offsetof(x86_saved_state_t, ss_64.rbp)
#define R64_RBX             0x78    // offsetof(x86_saved_state_t, ss_64.rbx)
#define R64_RCX             0x80    // offsetof(x86_saved_state_t, ss_64.rcx)
#define R64_RAX             0x88    // offsetof(x86_saved_state_t, ss_64.rax)
#define R64_GS              0x90    // offsetof(x86_saved_state_t, ss_64.gs)
#define R64_FS              0x94    // offsetof(x86_saved_state_t, ss_64.fs)
#define ISS64_OFFSET        0xA0    // offsetof(x86_saved_state_t, ss_64.isf)
#define R64_TRAPNO          0xA0    // offsetof(x86_saved_state_t, ss_64.isf.trapno)
#define R64_TRAPFN          0xA8    // offsetof(x86_saved_state_t, ss_64.isf.trapfn)
#define R64_ERR             0xB0    // offsetof(x86_saved_state_t, ss_64.isf.err)
#define R64_RIP             0xB8    // offsetof(x86_saved_state_t, ss_64.isf.rip)
#define R64_CS              0xC0    // offsetof(x86_saved_state_t, ss_64.isf.cs)
#define R64_RFLAGS          0xC8    // offsetof(x86_saved_state_t, ss_64.isf.rflags)
#define R64_RSP             0xD0    // offsetof(x86_saved_state_t, ss_64.isf.rsp)
#define R64_SS              0xD8    // offsetof(x86_saved_state_t, ss_64.isf.ss)

#define R32_GS              0x10    // offsetof(x86_saved_state_t, ss_32.gs)
#define R32_FS              0x14    // offsetof(x86_saved_state_t, ss_32.fs)
#define R32_ES              0x18    // offsetof(x86_saved_state_t, ss_32.es)
#define R32_DS              0x1C    // offsetof(x86_saved_state_t, ss_32.ds)
#define R32_EDI             0x20    // offsetof(x86_saved_state_t, ss_32.edi)
#define R32_ESI             0x24    // offsetof(x86_saved_state_t, ss_32.esi)
#define R32_EBP             0x28    // offsetof(x86_saved_state_t, ss_32.ebp)
#define R32_CR2             0x2C    // offsetof(x86_saved_state_t, ss_32.cr2)
#define R32_EBX             0x30    // offsetof(x86_saved_state_t, ss_32.ebx)
#define R32_EDX             0x34    // offsetof(x86_saved_state_t, ss_32.edx)
#define R32_ECX             0x38    // offsetof(x86_saved_state_t, ss_32.ecx)
#define R32_EAX             0x3C    // offsetof(x86_saved_state_t, ss_32.eax)
#define R32_TRAPNO          0x40    // offsetof(x86_saved_state_t, ss_32.trapno)
#define R32_CPU             0x42    // offsetof(x86_saved_state_t, ss_32.cpu)
#define R32_ERR             0x44    // offsetof(x86_saved_state_t, ss_32.err)
#define R32_EIP             0x48    // offsetof(x86_saved_state_t, ss_32.eip)
#define R32_CS              0x4C    // offsetof(x86_saved_state_t, ss_32.cs)
#define R32_EFLAGS          0x50    // offsetof(x86_saved_state_t, ss_32.efl)
#define R32_UESP            0x54    // offsetof(x86_saved_state_t, ss_32.uesp)
#define R32_SS              0x58    // offsetof(x86_saved_state_t, ss_32.ss)

#define CPU_TASK_MAP        0x10C   // offsetof(cpu_data_t, cpu_task_map)
#define CPU_TASK_CR3        0x110   // offsetof(cpu_data_t, cpu_task_cr3)
#define CPU_KERNEL_CR3      0x118   // offsetof(cpu_data_t, cpu_kernel_cr3)
#define CPU_ACTIVE_CR3      0x100   // offsetof(cpu_data_t, cpu_active_cr3)
#define CPU_TLB_INVALID     0x108   // offsetof(cpu_data_t, cpu_tlb_invalid)
#define CPU_TLB_INVALID_LOCAL 0x108 // offsetof(cpu_data_t, cpu_tlb_inavlid_local)
#define CPU_KERNEL_STACK    0x20    // offsetof(cpu_data_t, cpu_kernel_stack)
#define CPU_DR7             0x1628  // offsetof(cpu_data_t, cpu_dr7)
#define CPU_INT_STACK_TOP   0x28    // offsetof(cpu_data_t, cpu_int_stack_top)
#define CPU_INT_STATE       0x10    // offsetof(cpu_data_t, cpu_int_state)
#define CPU_UBER_ISF        0x120   // offsetof(cpu_data_t, cpu_uber.cu_isf)
#define CPU_UBER_TMP        0x128   // offsetof(cpu_data_t, cpu_uber.cu_tmp)
#define SS_FLAVOR           0x0     // offsetof(x86_saved_state_t, flavor)

#define hwIntCnt            0x224   // offsetof(cpu_data_t, cpu_hwIntCnt)

#define SS_64               x86_SAVED_STATE64
#define SS_32               x86_SAVED_STATE32
#define TASK_MAP_32BIT      0
#define TASK_MAP_64BIT      1

/*
 * Common dispatch point.
 * Determine what mode has been interrupted and save state accordingly.
 * Here with:
 *	rsp	from user-space:   interrupt state in PCB, or
 *		from kernel-space: interrupt state in kernel or interrupt stack
 *	GSBASE	from user-space:   pthread area, or
 *		from kernel-space: cpu_data
 */
L_dispatch:
	cmpl $(KERNEL64_CS), ISF64_CS(%rsp)
	je	 L_dispatch_kernel

	swapgs

L_dispatch_user:
	cmpl $(TASK_MAP_32BIT), %gs:CPU_TASK_MAP
	je	 L_dispatch_U32		// 32-bit user task

L_dispatch_U64:
	subq $(ISS64_OFFSET), %rsp
	mov	 %r15, R64_R15(%rsp)
	mov	 %rsp, %r15
	mov	 %gs:CPU_KERNEL_STACK, %rsp
	jmp	 L_dispatch_64bit

L_dispatch_kernel:
	subq $(ISS64_OFFSET), %rsp
	mov	 %r15, R64_R15(%rsp)
	mov	 %rsp, %r15

/*
 * Here for 64-bit user task or kernel
 */
 L_dispatch_64bit:
	movl $(SS_64), SS_FLAVOR(%r15)

	/*
	 * Save segment regs - for completeness since theyre not used.
	 */
	movl %fs, R64_FS(%r15)
	movl %gs, R64_GS(%r15)

	/* Save general-purpose registers */
	mov	%rax, R64_RAX(%r15)
	mov	%rbx, R64_RBX(%r15)
	mov	%rcx, R64_RCX(%r15)
	mov	%rdx, R64_RDX(%r15)
	mov	%rbp, R64_RBP(%r15)
	mov	%rdi, R64_RDI(%r15)
	mov	%rsi, R64_RSI(%r15)
	mov	%r8,  R64_R8(%r15)
	mov	%r9,  R64_R9(%r15)
	mov	%r10, R64_R10(%r15)
	mov	%r11, R64_R11(%r15)
	mov	%r12, R64_R12(%r15)
	mov	%r13, R64_R13(%r15)
	mov	%r14, R64_R14(%r15)

	/* cr2 is significant only for page-faults */
    mov	%cr2, %rax
	mov	%rax, R64_CR2(%r15)

	mov	R64_TRAPNO(%r15), %ebx	/* %ebx := trapno for later */
	mov	R64_TRAPFN(%r15), %rdx	/* %rdx := trapfn for later */
	mov	R64_CS(%r15), %esi      /* %esi := cs for later */

	//jmp	L_common_dispatch
L_64bit_entry_reject:
	/*
	 * Here for a 64-bit user attempting an invalid kernel entry.
	 */
	pushq	%rax
	//leaq	HNDL_ALLTRAPS(%rip), %rax
	movq	%rax, ISF64_TRAPFN+8(%rsp)
	popq	%rax
	movq	$(T_INVALID_OPCODE), ISF64_TRAPNO(%rsp)
	jmp 	L_dispatch_U64
	
L_32bit_entry_check:
	/*
	 * Check we're not a confused 64-bit user.
	 */
	cmpl	$(TASK_MAP_32BIT), %gs:CPU_TASK_MAP
	jne	L_64bit_entry_reject
	/* fall through to 32-bit handler: */

L_dispatch_U32: /* 32-bit user task */
	subq	$(ISS64_OFFSET), %rsp
	mov     %rsp, %r15
	mov     %gs:CPU_KERNEL_STACK, %rsp
	movl	$(SS_32), SS_FLAVOR(%r15)

    /*
	 * Save segment regs
	 */
	movl	%ds, R32_DS(%r15)
	movl	%es, R32_ES(%r15)
	movl	%fs, R32_FS(%r15)
	movl	%gs, R32_GS(%r15)

	/*
	 * Save general 32-bit registers
	 */
	mov	%eax, R32_EAX(%r15)
	mov	%ebx, R32_EBX(%r15)
	mov	%ecx, R32_ECX(%r15)
	mov	%edx, R32_EDX(%r15)
	mov	%ebp, R32_EBP(%r15)
	mov	%esi, R32_ESI(%r15)
	mov	%edi, R32_EDI(%r15)

	/* Unconditionally save cr2; only meaningful on page faults */
	mov	%cr2, %rax
	mov	%eax, R32_CR2(%r15)

	/*
	 * Copy registers already saved in the machine state 
	 * (in the interrupt stack frame) into the compat save area.
	 */
	mov	R64_RIP(%r15), %eax
	mov	%eax, R32_EIP(%r15)
	mov	R64_RFLAGS(%r15), %eax
	mov	%eax, R32_EFLAGS(%r15)
	mov	R64_RSP(%r15), %eax
	mov	%eax, R32_UESP(%r15)
	mov	R64_SS(%r15), %eax
	mov	%eax, R32_SS(%r15)

L_dispatch_U32_after_fault:
	mov	R64_CS(%r15), %esi          /* %esi := %cs for later */
	mov	%esi, R32_CS(%r15)
	mov	R64_TRAPNO(%r15), %ebx		/* %ebx := trapno for later */
	mov	%ebx, R32_TRAPNO(%r15)
	mov	R64_ERR(%r15), %eax
	mov	%eax, R32_ERR(%r15)
    mov	R64_TRAPFN(%r15), %rdx		/* %rdx := trapfn for later */

L_common_dispatch:
	cld         /* Ensure the direction flag is clear in the kernel */
	cmpl    $0, EXT(pmap_smap_enabled)(%rip)
	je	1f
	clac		/* Clear EFLAGS.AC if SMAP is present/enabled */
1:
	/*
	 * On entering the kernel, we don't need to switch cr3
	 * because the kernel shares the user's address space.
	 * But we mark the kernel's cr3 as "active".
	 * If, however, the invalid cr3 flag is set, we have to flush tlbs
	 * since the kernel's mapping was changed while we were in userspace.
	 *
	 * But: if global no_shared_cr3 is TRUE we do switch to the kernel's cr3
	 * so that illicit accesses to userspace can be trapped.
	 */
	mov	%gs:CPU_KERNEL_CR3, %rcx
	mov	%rcx, %gs:CPU_ACTIVE_CR3
	test	$3, %esi			/* user/kernel? */
	jz	2f                      /* skip cr3 reload from kernel */
	xor	%rbp, %rbp
	cmpl	$0, EXT(no_shared_cr3)(%rip)
	je	2f
	mov	%rcx, %cr3			/* load kernel cr3 */
	jmp	4f                  /* and skip tlb flush test */
2:
	mov	%gs:CPU_ACTIVE_CR3+4, %rcx
	shr	$32, %rcx
	testl	%ecx, %ecx
	jz	4f
	testl	$(1<<16), %ecx			/* Global? */
	jz	3f
	movl	$0, %gs:CPU_TLB_INVALID
	mov	%cr4, %rcx      /* RMWW CR4, for lack of an alternative*/
	and	$(~CR4_PGE), %rcx
	mov	%rcx, %cr4
	or	$(CR4_PGE), %rcx
	mov	%rcx, %cr4
	jmp	4f
3:
	movb	$0, %gs:CPU_TLB_INVALID_LOCAL
	mov	%cr3, %rcx
	mov	%rcx, %cr3
4:
	//mov	%gs:CPU_ACTIVE_THREAD, %rcx	/* Get the active thread */
	//movl	$-1, TH_IOTIER_OVERRIDE(%rcx)	/* Reset IO tier override to -1 before handling trap */
	//cmpq	$0, TH_PCB_IDS(%rcx)	/* Is there a debug register state? */
	//je	5f
	xor	%ecx, %ecx		/* If so, reset DR7 (the control) */
	mov	%rcx, %dr7
5:
	incl	%gs:hwIntCnt(,%ebx,4)		// Bump the trap/intr count
	/* Dispatch the designated handler */
	jmp	*%rdx

/*
 * Control is passed here to return to user.
 */
Entry(return_to_user)
	//TIME_TRAP_UEXIT   RTC_ASM.H

Entry(ret_to_user)
	//mov	%gs:CPU_ACTIVE_THREAD, %rdx
	//movq	TH_PCB_IDS(%rdx),%rax	/* Obtain this thread's debug state */
	
	test	%rax, %rax		/* Is there a debug register context? */
	je	2f                  /* branch if not */
	cmpl	$(TASK_MAP_32BIT), %gs:CPU_TASK_MAP /* Are we a 32-bit task? */
	jne	1f
	//movl	DS_DR0(%rax), %ecx                  /* If so, load the 32 bit DRs */
	movq	%rcx, %dr0
	//movl	DS_DR1(%rax), %ecx
	movq	%rcx, %dr1
	//movl	DS_DR2(%rax), %ecx
	movq	%rcx, %dr2
	//movl	DS_DR3(%rax), %ecx
	movq	%rcx, %dr3
	//movl	DS_DR7(%rax), %ecx
	movq 	%rcx, %gs:CPU_DR7
	jmp 	2f
1:
	//mov	DS64_DR0(%rax), %rcx	/* Load the full width DRs*/
	mov	%rcx, %dr0
	//mov	DS64_DR1(%rax), %rcx
	mov	%rcx, %dr1
	//mov	DS64_DR2(%rax), %rcx
	mov	%rcx, %dr2
	//mov	DS64_DR3(%rax), %rcx
	mov	%rcx, %dr3
	//mov	DS64_DR7(%rax), %rcx
	mov 	%rcx, %gs:CPU_DR7
2:
	/*
	 * On exiting the kernel there's no need to switch cr3 since we're
	 * already running in the user's address space which includes the
	 * kernel. Nevertheless, we now mark the task's cr3 as active.
	 * But, if no_shared_cr3 is set, we do need to switch cr3 at this point.
	 */
	mov	%gs:CPU_TASK_CR3, %rcx
	mov	%rcx, %gs:CPU_ACTIVE_CR3
	movl	EXT(no_shared_cr3)(%rip), %eax
	test	%eax, %eax		/* -no_shared_cr3 */
	jz	3f
	mov	%rcx, %cr3
3:
	mov	%gs:CPU_DR7, %rax	/* Is there a debug control register?*/
	cmp	$0, %rax
	je	4f
	mov	%rax, %dr7          /* Set DR7 */
	movq	$0, %gs:CPU_DR7
4:
	cmpl	$(SS_64), SS_FLAVOR(%r15)	/* 64-bit state? */
	je	L_64bit_return

L_32bit_return:
	/*
	 * Restore registers into the machine state for iret.
	 * Here on fault stack and PCB address in R11.
	 */
	movl	R32_EIP(%r15), %eax
	movl	%eax, R64_RIP(%r15)
	movl	R32_EFLAGS(%r15), %eax
	movl	%eax, R64_RFLAGS(%r15)
	movl	R32_CS(%r15), %eax
	movl	%eax, R64_CS(%r15)
	movl	R32_UESP(%r15), %eax
	movl	%eax, R64_RSP(%r15)
	movl	R32_SS(%r15), %eax
	movl	%eax, R64_SS(%r15)

	/*
	 * Restore general 32-bit registers
	 */
	movl	R32_EAX(%r15), %eax
	movl	R32_EBX(%r15), %ebx
	movl	R32_ECX(%r15), %ecx
	movl	R32_EDX(%r15), %edx
	movl	R32_EBP(%r15), %ebp
	movl	R32_ESI(%r15), %esi
	movl	R32_EDI(%r15), %edi

	/*
	 * Restore segment registers. A segment exception taken here will
	 * push state on the IST1 stack and will not affect the "PCB stack".
	 */
	mov	%r15, %rsp		/* Set the PCB as the stack */
	swapgs
EXT(ret32_set_ds):	
	movl	R32_DS(%rsp), %ds
EXT(ret32_set_es):
	movl	R32_ES(%rsp), %es
EXT(ret32_set_fs):
	movl	R32_FS(%rsp), %fs
EXT(ret32_set_gs):
	movl	R32_GS(%rsp), %gs

	/* pop compat frame + trapno, trapfn and error */	
	add	$(ISS64_OFFSET)+8+8+8, %rsp
	cmpl	$(SYSENTER_CS),ISF64_CS-8-8-8(%rsp) /* test for fast entry/exit */
	je      L_fast_exit
EXT(ret32_iret):
	iretq                       /* return from interrupt */

L_fast_exit:
	pop	 %rdx               /* user return eip */
	pop	 %rcx               /* pop and toss cs */
	andl $(~EFL_IF), (%rsp) /* clear interrupts enable, sti below */
	popf                    /* flags - carry denotes failure */
	pop	 %rcx               /* user return esp */
	sti                     /* interrupts enabled after sysexit */
	sysexitl                /* 32-bit sysexit */

ret_to_kernel:

L_64bit_return:
	/*
	 * Restore general 64-bit registers.
	 * Here on fault stack and PCB address in R15.
	 */
	mov	R64_R14(%r15), %r14
	mov	R64_R13(%r15), %r13
	mov	R64_R12(%r15), %r12
	mov	R64_R11(%r15), %r11
	mov	R64_R10(%r15), %r10
	mov	R64_R9(%r15),  %r9
	mov	R64_R8(%r15),  %r8
	mov	R64_RSI(%r15), %rsi
	mov	R64_RDI(%r15), %rdi
	mov	R64_RBP(%r15), %rbp
	mov	R64_RDX(%r15), %rdx
	mov	R64_RCX(%r15), %rcx
	mov	R64_RBX(%r15), %rbx
	mov	R64_RAX(%r15), %rax

	/*
	 * We must swap GS base if we're returning to user-space,
	 * or we're returning from an NMI that occurred in a trampoline
	 * before the user GS had been swapped. In the latter case, the NMI
	 * handler will have flagged the high-order 32-bits of the CS.
	 */
	cmpq	$(KERNEL64_CS), R64_CS(%r15)
	jz	1f
	swapgs
1:
	mov	R64_R15(%r15), %rsp
	xchg	%r15, %rsp
	add	$(ISS64_OFFSET)+24, %rsp          /* pop saved state          */
                                          /* + trapno/trapfn/error    */
	cmpl $(SYSCALL_CS),ISF64_CS-24(%rsp)  /* test for fast entry/exit */
	je   L_sysret
.globl _dump_iretq
EXT(ret64_iret):
        iretq    /* return from interrupt */

L_sysret:
	/*
	 * Here to load rcx/r11/rsp and perform the sysret back to user-space.
	 * 	rcx	user rip
	 *	r11	user rflags
	 *	rsp	user stack pointer
	 */
	mov	ISF64_RIP-24(%rsp), %rcx
	mov	ISF64_RFLAGS-24(%rsp), %r11
	mov	ISF64_RSP-24(%rsp), %rsp
    sysretq      /* return from sys call */



/*
 * System call handlers.
 * These are entered via a syscall interrupt. The system call number in %rax
 * is saved to the error code slot in the stack frame. We then branch to the
 * common state saving code.
 */
Entry(idt64_unix_scall)
    swapgs                  /* switch to kernel gs (cpu_data) */
    pushq %rax              /* save system call number */
    //PUSH_FUNCTION(HNDL_UNIX_SCALL)
    pushq $(UNIX_INT)
    jmp	  L_32bit_entry_check

	
Entry(idt64_mach_scall)
    swapgs                  /* switch to kernel gs (cpu_data) */
    pushq   %rax            /* save system call number */
    //PUSH_FUNCTION(HNDL_MACH_SCALL)
    pushq   $(MACH_INT)
    jmp	L_32bit_entry_check

Entry(idt64_mdep_scall)
    swapgs                  /* switch to kernel gs (cpu_data) */
    pushq   %rax            /* save system call number */
    //PUSH_FUNCTION(HNDL_MDEP_SCALL)
    pushq   $(MACHDEP_INT)
    jmp	L_32bit_entry_check

 /* Programmed into MSR_IA32_LSTAR by mp_desc.c */
Entry(hi64_syscall)
Entry(idt64_syscall)
L_syscall_continue:
	swapgs				/* Kapow! get per-cpu data area */
	mov	%rsp, %gs:CPU_UBER_TMP	/* save user stack */
	mov	%gs:CPU_UBER_ISF, %rsp	/* switch stack to pcb */

	/*
	 * Save values in the ISF frame in the PCB
	 * to cons up the saved machine state.
	 */
	movl $(USER_DS), ISF64_SS(%rsp)
	movl $(SYSCALL_CS), ISF64_CS(%rsp)      /* cs - a pseudo-segment */
	mov	 %r11, ISF64_RFLAGS(%rsp)           /* rflags */
	mov	 %rcx, ISF64_RIP(%rsp)              /* rip */
    mov	 %gs:CPU_UBER_TMP, %rcx
	mov	 %rcx, ISF64_RSP(%rsp)              /* user stack */
	mov	 %rax, ISF64_ERR(%rsp)              /* err/rax - syscall code */
	movq $(T_SYSCALL), ISF64_TRAPNO(%rsp)	/* trapno */
	//leaq HNDL_SYSCALL(%rip), %r11
	movq %r11, ISF64_TRAPFN(%rsp)
	mov	ISF64_RFLAGS(%rsp), %r11            /* Avoid leak, restore R11 */
	jmp	L_dispatch_U64                      /* this can only be 64-bit */
	
/*
 * sysenter entry point
 * Requires user code to set up:
 *	edx: user instruction pointer (return address)
 *	ecx: user stack pointer
 *		on which is pushed stub ret addr and saved ebx
 * Return to user-space is made using sysexit.
 * Note: sysenter/sysexit cannot be used for calls returning a value in edx,
 *       or requiring ecx to be preserved.
 */
Entry(hi64_sysenter)
Entry(idt64_sysenter)
	movq	(%rsp), %rsp
	/*
	 * Push values on to the PCB stack
	 * to cons up the saved machine state.
	 */
	push	$(USER_DS)		/* ss */
	push	%rcx			/* uesp */
	pushf				/* flags */
	/*
	 * Clear, among others, the Nested Task (NT) flags bit;
	 * this is zeroed by INT, but not by SYSENTER.
	 */
	push	$0
	popf
	push	$(SYSENTER_CS)		/* cs */ 
L_sysenter_continue:
	swapgs                  /* switch to kernel gs (cpu_data) */
	push	%rdx			/* eip */
	push	%rax			/* err/eax - syscall code */
	//PUSH_FUNCTION(HNDL_SYSENTER)
	pushq	$(T_SYSENTER)
	orl	$(EFL_IF), ISF64_RFLAGS(%rsp)
	jmp	L_32bit_entry_check




Entry(hndl_unix_scall64)
	//incl TH_SYSCALLS_UNIX(%rcx)       /* increment call count   */
	sti
	CCALL1(unix_syscall64, %r15)  // Not yet implemented
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mach_scall64)
	//incl TH_SYSCALLS_MACH(%rcx)     /* increment call count   */
	sti
	//CCALL1(mach_call_munger64, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mdep_scall64)
	sti
	//CCALL1(machdep_syscall64, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_diag_scall64)
	//CCALL1(diagCall64, %r15)	// Call diagnostics
	test    %eax, %eax          // What kind of return is this?
	je      1f                  // branch if bad (zero)
	jmp	EXT(return_to_user)     // Normal return, do not check asts...
1:
	sti
	//CCALL3(i386_exception, $EXC_SYSCALL, $0x6000, $1)
	/* no return */

Entry(hndl_machine_check)
	//CCALL1(panic_machine_check64, %r15)
	hlt

Entry(hndl_double_fault)
	//CCALL1(panic_double_fault64, %r15)
	hlt
