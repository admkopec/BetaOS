//
//  low_mem_vectors.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "lowglobals.h"
#include <i386/vm_param.h>

// To be changed in the future

/*
 * on x86_64 the low mem vectors live here and get mapped to 0xffffff8000002000 at
 * system startup time
 */

extern void	*version;
extern void	*kmod;
extern void	*kdp_trans_off;
extern void	*kdp_read_io;
extern void	*osversion;
extern void	*flag_kdp_trigger_reboot;
extern void	*manual_pkt;

lowglo lowGlo __attribute__ ((aligned(PAGE_SIZE))) = {
    
    .lgVerCode		= { 'C','a','t','f','i','s','h',' ' },
    
    .lgCHUDXNUfnStart	= 0,
    
    //.lgVersion		= (uint64_t) &version,
    
    //.lgKmodptr		= (uint64_t) &kmod,
    
    .lgTransOff		= (uint64_t) &kdp_trans_off,
    .lgReadIO		= (uint64_t) &kdp_read_io,
    
    .lgDevSlot1		= 0,
    .lgDevSlot2		= 0,
    
    //.lgOSVersion		= (uint64_t) &osversion,
    
    //.lgRebootFlag		= (uint64_t) &flag_kdp_trigger_reboot,
    //.lgManualPktAddr	= (uint64_t) &manual_pkt,
    .lgRebootFlag		= 0,
    .lgManualPktAddr	= 0,
};
