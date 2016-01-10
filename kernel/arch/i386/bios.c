//
//  bios.c
//  OS
//
//  Created by Adam Kopeć on 1/10/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <kernel/bios.h>
#include <string.h>

/* Temperary () */

static inline bool
Intr_Save(void) {
    uint32 eflags;
    asm volatile ("pushf; pop %0" : "=r" (eflags));
    return (eflags & 0x200) != 0;
}

static inline void
Intr_Restore(bool flag) {
    if (flag) {
        asm volatile ("sti");
    } else {
        asm volatile ("cli");
    }
}

/*
 * BIOSCallInternal --
 *
 *    Internal implementation of BIOS_Call. This is a C function which
 *    wraps the assembly-language internal implementation. The only
 *    reason to use C here, really, is so we can easily calculate
 *    offsets into our shared C structures.
 *
 *    This function must not make any function calls, since we need to
 *    be able to trust the value of %esp. This is also why it must not
 *    be inlined into BIOS_Call itself.
 */

static __attribute__((noinline)) void
BIOSCallInternal(void)
{
    /*
     * Save registers and stack in a safe place.
     */
    asm volatile ("pusha");
    asm volatile ("mov %%esp, %0" :"=m" (BIOS_SHARED->esp));
    
    /*
     * Jump the the relocated 16-bit trampoline (source code below).
     */
    asm volatile ("ljmp %0, %1"
                  :: "i" (BOOT_CODE16_SEG), "i" (BIOS_SHARED->trampoline));
    
    /*
     * This is where we return from the relocated trampoline.
     * We're back in protected mode, but the data segments
     * are still 16-bit. Restore them.
     */
    
    asm volatile ("BIOSReturn32: \n"
                  "mov %0, %%ax \n"
                  "mov %%ax, %%ss \n"
                  "mov %%ax, %%ds \n"
                  "mov %%ax, %%es \n"
                  "mov %%ax, %%fs \n"
                  "mov %%ax, %%gs \n"
                  :: "i" (BOOT_DATA_SEG));
    
    /*
     * Restore our stack and saved registers.
     * Now we can safely execute C code again.
     */
    
    asm volatile("mov %0, %%esp" ::"m" (BIOS_SHARED->esp));
    asm volatile ("popa");
    
    /*
     * Return here. The rest of this code is never run directly,
     * but we need to prevent GCC from optimizing it out.
     */
    asm volatile("jmp BIOSTrampolineEnd\n");
    
    /*
     * This is a 16-bit assembly-language trampoline, relocated at
     * runtime to low memory, which actually makes the BIOS call. It
     * handles saving/restoring registers, and it switches in and out
     * of real mode.
     *
     * This code is never run directly.
     */
    
    asm volatile("BIOSTrampoline: .code16");
    
    /*
     * Switch to our 16-bit data segment.
     */
    asm volatile("movw %0, %%ax \n"
                 "movw %%ax, %%ds \n"
                 "movw %%ax, %%es \n"
                 "movw %%ax, %%ss \n"
                 :: "i" (BOOT_DATA16_SEG));
    
    /*
     * Disable protected mode.
     */
    asm volatile("movl %cr0, %eax \n"
                 "andl $(~1), %eax \n"
                 "movl %eax, %cr0 \n");
    
    /*
     * Do another long jump to reset the real-mode %cs
     * register to a valid paragraph number. Right now
     * it's still a protected-mode-style selector index.
     *
     * XXX: I'm not sure how to do this address calculation cleanly.
     *      Currently I'm hardcoding the address of the relocated trampoline.
     */
    asm volatile("ljmp $0, $(BIOSTrampolineCS16 - BIOSTrampoline + 0x7C00)\n"
                 "BIOSTrampolineCS16: \n");
    
    /*
     * Set up the real-mode stack and %cs register.
     */
    asm volatile("xorw %%ax, %%ax \n"
                 "mov %%ax, %%ss \n"
                 "mov %0, %%esp \n"
                 :: "i" (&BIOS_SHARED->stackTop[-sizeof(Regs)]));
    
    /*
     * Pop Regs off the stack.
     */
    asm volatile("pop %ds \n"
                 "pop %es \n"
                 "pop %eax \n"   // Ignore EFLAGS value.
                 "popal \n");
    
    /*
     * This interrupt instruction is a placeholder that gets
     * patched at runtime (after relocation) to point to the
     * right interrupt vector.
     */
    asm volatile("BIOSTrampolineVector: \n"
                 "int $0xFF");
    
    /*
     * Some BIOS routines will turn interrupts back on. If an interrupt
     * occurs while we're partway through our 16-to-32 transition
     * below, any interrupts will cause the ISR to fault, so we must
     * make sure interrupts are still off.
     */
    asm volatile("cli");
    
    /*
     * Push Regs back onto the stack.
     */
    asm volatile("pushal \n"
                 "pushfl \n"
                 "push %es \n"
                 "push %ds \n");
    
    /*
     * Enable protected mode.
     */
    asm volatile("movl %cr0, %eax \n"
                 "orl $1, %eax \n"
                 "movl %eax, %cr0 \n");
    
    /*
     * Return via a long 16-to-32 bit jump.
     */
    asm volatile("data32 ljmp %0, $BIOSReturn32 \n"
                 :: "i" (BOOT_CODE_SEG));
    
    asm volatile("BIOSTrampolineEnd: .code32 \n");
}

extern struct {
    uint16 limit;
    uint32 base;
} PACKED IDTDesc;

/*
 * BIOS_Call --
 *
 *    Make BIOS calls after boot, by temporarily switching
 *    back into real mode.
 *
 *    This function relocates the trampoline and stack into
 *    real-mode-addressable low memory, then makes a 32-to-16-bit jump
 *    into the trampoline.
 */

fastcall void
BIOS_Call(uint8 vector, Regs *regs)
{
    extern uint8 BIOSTrampoline[];
    extern uint8 BIOSTrampolineVector[];
    extern uint8 BIOSTrampolineEnd[];
    const uint32 trampSize = (uint8*)BIOSTrampolineEnd - (uint8*)BIOSTrampoline;
    const uint32 vectorOffset = (uint8*)BIOSTrampolineVector - (uint8*)BIOSTrampoline + 1;
    
    bool iFlag = Intr_Save();
    asm volatile ("cli");
    
    /*
     * Relocate the trampoline code itself.
     */
    memcpy(BIOS_SHARED->trampoline, BIOSTrampoline, trampSize);
    
    /*
     * Save the 32-bit IDT descriptor, and set up a legacy 256-entry
     * 16-bit IDT descriptor.
     */
    asm volatile("sidt %0" : "=m" (BIOS_SHARED->idtr32));
    BIOS_SHARED->idtr16.base = 0;
    BIOS_SHARED->idtr16.limit = 0x3ff;
    asm volatile("lidt %0" :: "m" (BIOS_SHARED->idtr16));
    
    /*
     * Binary-patch the trampoline code with the right interrupt vector.
     */
    BIOS_SHARED->trampoline[vectorOffset] = vector;
    
    /*
     * Copy Regs onto the top of the 16-bit stack.
     */
    memcpy(&BIOS_SHARED->stackTop[-sizeof *regs], regs, sizeof *regs);
    
    BIOSCallInternal();
    
    /* Copy Regs back */
    memcpy(regs, &BIOS_SHARED->stackTop[-sizeof *regs], sizeof *regs);
    
    /*
     * Back to 32-bit IDT.
     */
    asm volatile("lidt %0" :: "m" (BIOS_SHARED->idtr32));
    
    Intr_Restore(iFlag);
}
