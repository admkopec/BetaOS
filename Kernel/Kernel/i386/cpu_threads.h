//
//  cpu_threads.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef cpu_threads_h
#define cpu_threads_h

#include <i386/cpu_data.h>

/* Fast access: */
#define x86_lcpu()			(&current_cpu_datap()->lcpu)
#define x86_core()			(x86_lcpu()->core)
#define x86_die()			(x86_lcpu()->die)
#define x86_package()		(x86_lcpu()->package)

extern x86_topology_parameters_t	topoParms;

#endif /* cpu_threads_h */
