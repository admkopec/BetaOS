//
//  cpu_threads.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "cpu_threads.h"
#include "cpu_data.h"
#include "cpuid.h"
#include "cpu_topology.h"

x86_pkg_t                   *x86_pkgs		= NULL;
uint32_t                    num_Lx_caches[MAX_CACHE_DEPTH]	= { 0 };

__unused static x86_pkg_t            *free_pkgs	= NULL;
__unused static x86_die_t            *free_dies	= NULL;
__unused static x86_core_t           *free_cores	= NULL;
__unused static uint32_t             num_dies	= 0;

__unused static x86_cpu_cache_t      *x86_caches	= NULL;
__unused static uint32_t             num_caches	= 0;

__unused static bool                 topoParmsInited	= FALSE;
x86_topology_parameters_t	topoParms;