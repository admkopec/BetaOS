//
//  lapic.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef lapic_h
#define lapic_h

#define 	LAPIC_TIMER_DIVIDE_1	0x0000000B
#define 	LAPIC_TIMER_DIVIDE_2	0x00000000
#define 	LAPIC_TIMER_DIVIDE_4	0x00000001
#define 	LAPIC_TIMER_DIVIDE_8	0x00000002
#define 	LAPIC_TIMER_DIVIDE_16	0x00000003
#define 	LAPIC_TIMER_DIVIDE_32	0x00000008
#define 	LAPIC_TIMER_DIVIDE_64	0x00000009
#define 	LAPIC_TIMER_DIVIDE_128	0x0000000A
#include <stdint.h>

typedef enum {
    ID                  = 0x02,
    VERSION             = 0x03,
    TPR                 = 0x08,
    APR                 = 0x09,
    PPR                 = 0x0A,
    EOI                 = 0x0B,
    REMOTE_READ         = 0x0C,
    LDR                 = 0x0D,
    DFR                 = 0x0E,
    SVR                 = 0x0F,
    ISR_BASE            = 0x10,
    TMR_BASE            = 0x18,
    IRR_BASE            = 0x20,
    ERROR_STATUS		= 0x28,
    LVT_CMCI            = 0x2F,
    ICR                 = 0x30,
    ICRD                = 0x31,
    LVT_TIMER           = 0x32,
    LVT_THERMAL         = 0x33,
    LVT_PERFCNT         = 0x34,
    LVT_LINT0           = 0x35,
    LVT_LINT1           = 0x36,
    LVT_ERROR           = 0x37,
    TIMER_INITIAL_COUNT	= 0x38,
    TIMER_CURRENT_COUNT	= 0x39,
    TIMER_DIVIDE_CONFIG	= 0x3E,
} lapic_register_t;

#define LAPIC_MMIO_PBASE	0xFEE00000	/* Default physical MMIO addr */
#define LAPIC_MMIO_VBASE	lapic_vbase	/* Actual virtual mapped addr */
#define LAPIC_MSR_BASE		0x800

#define	LAPIC_MMIO_OFFSET(reg)	(reg << 4)
#define	LAPIC_MSR_OFFSET(reg)	(reg)

#define	LAPIC_MMIO(reg)		((volatile uint32_t *) \
                            (LAPIC_MMIO_VBASE + LAPIC_MMIO_OFFSET(reg)))
#define	LAPIC_MSR(reg)		(LAPIC_MSR_BASE   + LAPIC_MSR_OFFSET(reg))

typedef struct {
    void		(*init)		(void);
    uint32_t	(*read)		(lapic_register_t);
    void		(*write)	(lapic_register_t, uint32_t);
    uint64_t	(*read_icr)	(void);
    void		(*write_icr)(uint32_t, uint32_t);
} lapic_ops_table_t;
extern  lapic_ops_table_t *lapic_ops;

#define LAPIC_INIT()                lapic_ops->init();
#define LAPIC_WRITE(reg,val)		lapic_ops->write(reg, val)
#define LAPIC_READ(reg)             lapic_ops->read(reg)
#define LAPIC_READ_OFFSET(reg,off)	LAPIC_READ((reg)+(off))
#define LAPIC_READ_ICR()            lapic_ops->read_icr()
#define LAPIC_WRITE_ICR(dst,cmd)	lapic_ops->write_icr(dst, cmd)

typedef enum {
    periodic,
    one_shot
} lapic_timer_mode_t;
typedef enum {
    divide_by_1   = LAPIC_TIMER_DIVIDE_1,
    divide_by_2   = LAPIC_TIMER_DIVIDE_2,
    divide_by_4   = LAPIC_TIMER_DIVIDE_4,
    divide_by_8   = LAPIC_TIMER_DIVIDE_8,
    divide_by_16  = LAPIC_TIMER_DIVIDE_16,
    divide_by_32  = LAPIC_TIMER_DIVIDE_32,
    divide_by_64  = LAPIC_TIMER_DIVIDE_64,
    divide_by_128 = LAPIC_TIMER_DIVIDE_128
} lapic_timer_divide_t;
typedef uint32_t lapic_timer_count_t;

/*
 * By default, use high vectors to leave vector space for systems
 * with multiple I/O APIC's. However some systems that boot with
 * local APIC disabled will hang in SMM when vectors greater than
 * 0x5F are used. Those systems are not expected to have I/O APIC
 * so 16 (0x50 - 0x40) vectors for legacy PIC support is perfect.
 */
#define LAPIC_DEFAULT_INTERRUPT_BASE	0xD0
#define LAPIC_REDUCED_INTERRUPT_BASE	0x50
/*
 * Specific lapic interrupts are relative to this base
 * in priority order from high to low:
 */

#define LAPIC_PERFCNT_INTERRUPT         0xF
#define LAPIC_INTERPROCESSOR_INTERRUPT	0xE
#define LAPIC_TIMER_INTERRUPT           0xD
#define LAPIC_THERMAL_INTERRUPT         0xC
#define LAPIC_ERROR_INTERRUPT           0xB
#define LAPIC_SPURIOUS_INTERRUPT        0xA
#define LAPIC_CMCI_INTERRUPT            0x9
#define LAPIC_PMC_SW_INTERRUPT          0x8
#define LAPIC_PM_INTERRUPT              0x7
#define LAPIC_KICK_INTERRUPT            0x6

#define LAPIC_PMC_SWI_VECTOR		(LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_PMC_SW_INTERRUPT)
#define LAPIC_TIMER_VECTOR          (LAPIC_DEFAULT_INTERRUPT_BASE + LAPIC_TIMER_INTERRUPT)

/* The vector field is ignored for NMI interrupts via the LAPIC
 * or otherwise, so this is not an offset from the interrupt
 * base.
 */
#define LAPIC_NMI_INTERRUPT     0x2
#define LAPIC_FUNC_TABLE_SIZE	(LAPIC_PERFCNT_INTERRUPT + 1)

#define LAPIC_VECTOR(src) \
    (lapic_interrupt_base + LAPIC_##src##_INTERRUPT)

#define LAPIC_ISR_IS_SET(base,src) \
    (LAPIC_READ_OFFSET(ISR_BASE,(base+LAPIC_##src##_INTERRUPT)/32) \
    & (1 <<((base + LAPIC_##src##_INTERRUPT)%32)))

extern int		lapic_to_cpu[];
extern int		cpu_to_lapic[];

#define	MAX_LAPICIDS	(0xFF+1)
#ifdef DEBUG
#define LAPIC_CPU_MAP_DUMP()	lapic_cpu_map_dump()
#define LAPIC_DUMP()		lapic_dump()
#else
#define LAPIC_CPU_MAP_DUMP()
#define LAPIC_DUMP()
#endif /* DEBUG */
#endif /* lapic_h */
