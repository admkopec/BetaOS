//
//  lapic.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/12/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include "lapic.h"
#include "mp.h"
#include "misc_protos.h"
#include "proc_reg.h"
#include "cpuid.h"
#include "cpu_data.h"
#include "vm_map.h"
#include "pmap.h"

lapic_ops_table_t       *lapic_ops;     /* Lapic operations switch */
static vm_map_offset_t	lapic_pbase;	/* Physical base memory-mapped regs */
static vm_offset_t      lapic_vbase;	/* Virtual base memory-mapped regs */
//static i386_intr_func_t	lapic_intr_func[LAPIC_FUNC_TABLE_SIZE];
/* TRUE if local APIC was enabled by the OS not by the EFI */
//static bool     lapic_os_enabled = FALSE;
//static bool     lapic_errors_masked = FALSE;
//static uint64_t lapic_last_master_error = 0;
//static uint64_t lapic_error_time_threshold = 0;
//static unsigned lapic_master_error_count = 0;
//static unsigned lapic_error_count_threshold = 5;
//static bool     lapic_dont_panic = FALSE;
/* Base vector for local APIC interrupt sources */
int lapic_interrupt_base = LAPIC_DEFAULT_INTERRUPT_BASE;
int		lapic_to_cpu[MAX_LAPICIDS];
int		cpu_to_lapic[MAX_CPUS];

void
lapic_cpu_map_init(void) {
	int	i;
    
	for (i = 0; i < MAX_CPUS; i++)
		cpu_to_lapic[i] = -1;
	for (i = 0; i < MAX_LAPICIDS; i++)
		lapic_to_cpu[i] = -1;
}

void
lapic_cpu_map(int apic_id, int cpu) {
	assert(apic_id < MAX_LAPICIDS);
	assert(cpu < MAX_CPUS);
	cpu_to_lapic[cpu] = apic_id;
	lapic_to_cpu[apic_id] = cpu;
}

/*
 * Retrieve the local apic ID a cpu.
 *
 * Returns the local apic ID for the given processor.
 * If the processor does not exist or apic not configured, returns -1.
 */

uint32_t
ml_get_apicid(uint32_t cpu) {
	if(cpu >= (uint32_t)MAX_CPUS)
		return 0xFFFFFFFF;	/* Return -1 if cpu too big */
	
	/* Return the apic ID (or -1 if not configured) */
	return (uint32_t)cpu_to_lapic[cpu];
    
}

uint32_t
ml_get_cpuid(uint32_t lapic_index) {
	if(lapic_index >= (uint32_t)MAX_LAPICIDS)
		return 0xFFFFFFFF;	/* Return -1 if cpu too big */
	
	/* Return the cpu ID (or -1 if not configured) */
	return (uint32_t)lapic_to_cpu[lapic_index];
    
}

#ifdef DEBUG
void
lapic_cpu_map_dump(void) {
	int	i;
	for (i = 0; i < MAX_CPUS; i++) {
		if (cpu_to_lapic[i] == -1)
			continue;
		kprintf("cpu_to_lapic[%d]: %d\n", i, cpu_to_lapic[i]);
	}
	for (i = 0; i < MAX_LAPICIDS; i++) {
		if (lapic_to_cpu[i] == -1)
			continue;
		kprintf("lapic_to_cpu[%d]: %d\n", i, lapic_to_cpu[i]);
	}
}
#endif /* DEBUG */

static void
legacy_init(void) {
    __unused int             result;
    __unused vm_map_entry_t  entry;
    __unused vm_map_offset_t lapic_vbase64;
    
    if (lapic_vbase == 0) {
        //lapic_vbase64 = (vm_map_offset_t)vm_map_min(kernel_map);
    }
    
    LAPIC_WRITE(DFR, LAPIC_DFR_FLAT);
    LAPIC_WRITE(LDR, (get_cpu_number()) << LAPIC_LDR_SHIFT);
}

static uint32_t
legacy_read(lapic_register_t reg) {
    return  *LAPIC_MMIO(reg);
}

static void
legacy_write(lapic_register_t reg, uint32_t value) {
    *LAPIC_MMIO(reg) = value;
}

static uint64_t
legacy_read_icr(void) {
    return (((uint64_t) *LAPIC_MMIO(ICRD)) << 32) | ((uint64_t) *LAPIC_MMIO(ICR));
}

static void
legacy_write_icr(uint32_t dst, uint32_t cmd) {
    *LAPIC_MMIO(ICRD) = dst << LAPIC_ICRD_DEST_SHIFT;
    *LAPIC_MMIO(ICR)  = cmd;
}

static lapic_ops_table_t legacy_ops = {
    legacy_init,
    legacy_read,
    legacy_write,
    legacy_read_icr,
    legacy_write_icr
};

static bool is_x2apic = false;

static void
x2apic_init(void) {
	uint32_t	low;
	uint32_t	high;
    
	rdmsr(MSR_IA32_APIC_BASE, low, high);
	if ((low  & MSR_IA32_APIC_BASE_EXTENDED) == 0)  {
		 low |= MSR_IA32_APIC_BASE_EXTENDED;
		wrmsr(MSR_IA32_APIC_BASE, low, high);
		kprintf("x2APIC mode enabled\n");
	}
}

static uint32_t
x2apic_read(lapic_register_t reg) {
	uint32_t	low;
	uint32_t	high;
    
	rdmsr(LAPIC_MSR(reg), low, high);
	return low;
}

static void
x2apic_write(lapic_register_t reg, uint32_t value) {
	wrmsr(LAPIC_MSR(reg), value, 0);
}

static uint64_t
x2apic_read_icr(void) {
	return rdmsr64(LAPIC_MSR(ICR));;
}

static void
x2apic_write_icr(uint32_t dst, uint32_t cmd) {
    wrmsr(LAPIC_MSR(ICR), cmd, dst);
}

static lapic_ops_table_t x2apic_ops = {
	x2apic_init,
	x2apic_read,
	x2apic_write,
	x2apic_read_icr,
	x2apic_write_icr
};

void
lapic_init(void) {
    uint32_t low;
    uint32_t high;
    bool     is_boot_cpu;
    bool     is_lapic_enabled;
    
    rdmsr(MSR_IA32_APIC_BASE, low, high);
    is_boot_cpu      = (low & MSR_IA32_APIC_BASE_BSP)      != 0;
    is_lapic_enabled = (low & MSR_IA32_APIC_BASE_ENABLE)   != 0;
    is_x2apic        = (low & MSR_IA32_APIC_BASE_EXTENDED) != 0;
    lapic_pbase      = (low & MSR_IA32_APIC_BASE_BASE);
    
    kprintf("MSR_IA32_APIC_BASE 0x%llx %s %s mode %s\n", lapic_pbase, is_lapic_enabled ? "enabled" : "disabled", is_x2apic ? "extended" : "legacy", is_boot_cpu ? "BSP" : "AP");
    if (!is_boot_cpu || !is_lapic_enabled) {
        panic("Unexpected local APIC state\n");
    }
    
    // If x2APIC is available and not enabled, enable it
    if (!is_x2apic && (cpuid_features() & CPUID_FEATURE_x2APIC)) {
        kprintf("x2APIC supported and will be enabled\n");
    }
    
    lapic_ops = is_x2apic ? &x2apic_ops : &legacy_ops;
    
    LAPIC_INIT();
    
    kprintf("ID: 0x%x LDR: 0x%x\n", LAPIC_READ(ID), LAPIC_READ(LDR));
    if ((LAPIC_READ(VERSION) & LAPIC_VERSION_MASK) < 0x14) {
        panic("Local APIC version 0x%x, 0x14 or more expected\n", (LAPIC_READ(VERSION) & LAPIC_VERSION_MASK));
    }
    
    lapic_cpu_map_init();
    lapic_cpu_map((LAPIC_READ(ID) >> LAPIC_ID_SHIFT) & LAPIC_ID_MASK, 0);
    current_cpu_datap()->cpu_phys_number = cpu_to_lapic[0];
    kprintf("Boot CPU Local APIC ID 0x%x\n", cpu_to_lapic[0]);
}
