//
//  locks.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/28/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef locks_h
#define locks_h

#include <stdint.h>
#include <i386/vm_types.h>

typedef struct _lck_mtx_ {
    union {
        struct {
            volatile unsigned long	lck_mtxd_owner;
            union {
                struct {
                    volatile uint32_t
                lck_mtxd_waiters:16,
                lck_mtxd_pri:8,
                lck_mtxd_ilocked:1,
                lck_mtxd_mlocked:1,
                lck_mtxd_promoted:1,
                lck_mtxd_spin:1,
                lck_mtxd_is_ext:1,
                lck_mtxd_pad3:3;
                };
                uint32_t	lck_mtxd_state;
            };
            /* Pad field used as a canary, initialized to ~0 */
            uint32_t			lck_mtxd_pad32;
        } lck_mtxd;
        struct {
            struct _lck_mtx_ext_		*lck_mtxi_ptr;
            uint32_t			lck_mtxi_tag;
            uint32_t			lck_mtxi_pad32;
        } lck_mtxi;
    } lck_mtx_sw;
} lck_mtx_t;


typedef struct {
    unsigned int		type;
    unsigned int		pad4;
    vm_offset_t         pc;
    vm_offset_t         thread;
} lck_mtx_deb_t;

#define MUTEX_TAG       0x4d4d

typedef struct {
    unsigned int		lck_mtx_stat_data;
} lck_mtx_stat_t;

typedef struct _lck_mtx_ext_ {
    lck_mtx_t           lck_mtx;
    struct _lck_grp_	*lck_mtx_grp;
    unsigned int		lck_mtx_attr;
    unsigned int		lck_mtx_pad1;
    lck_mtx_deb_t		lck_mtx_deb;
    uint64_t            lck_mtx_stat;
    unsigned int		lck_mtx_pad2[2];
} lck_mtx_ext_t;

#pragma pack(1)		/* Make sure the structure stays as we defined it */
typedef struct _lck_rw_t_internal_ {
    volatile uint16_t	lck_rw_shared_count;	/* No. of accepted readers */
    volatile uint8_t	lck_rw_interlock;       /* Interlock byte */
    volatile uint8_t
                        lck_rw_priv_excl:1,     /* Writers prioritized if set */
                        lck_rw_want_upgrade:1,	/* Read-to-write upgrade waiting */
                        lck_rw_want_write:1,	/* Writer waiting or locked for write */
                        lck_r_waiting:1,        /* Reader is sleeping on lock */
                        lck_w_waiting:1,        /* Writer is sleeping on lock */
                        lck_rw_can_sleep:1,     /* Can attempts to lock go to sleep? */
                        lck_rw_padb6:2; 		/* padding */
    
    uint32_t            lck_rw_tag; /* This can be obsoleted when stats
                                     * are in
                                     */
    uint32_t            lck_rw_pad8;
    uint32_t            lck_rw_pad12;
} lck_rw_t;
#pragma pack()

#define	LCK_RW_ATTR_DEBUG       0x1
#define	LCK_RW_ATTR_DEBUGb      0
#define	LCK_RW_ATTR_STAT        0x2
#define	LCK_RW_ATTR_STATb       1
#define LCK_RW_ATTR_READ_PRI	0x3
#define LCK_RW_ATTR_READ_PRIb	2
#define	LCK_RW_ATTR_DIS_THREAD	0x40000000
#define	LCK_RW_ATTR_DIS_THREADb	30
#define	LCK_RW_ATTR_DIS_MYLOCK	0x10000000
#define	LCK_RW_ATTR_DIS_MYLOCKb	28

#define	LCK_RW_TAG_DESTROYED	0x00002007	/* lock marked as Destroyed */

extern void		kernel_preempt_check (void);

#endif /* locks_h */
