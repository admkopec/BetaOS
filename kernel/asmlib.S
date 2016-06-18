//
//  asmlib.s
//  BetaOS
//
//  Created by Adam Kopeć on 3/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <i386/asm.h>


    /* halt_cpu */

Entry(halt_cpu)
    sti
    hlt

    ret

    /* x86_triplefault */

Entry(x86_triplefault)
    lidt    idt_zero
    int $3  /* Just to interrupt, CPU won't like it */
.data
idt_zero:
.long   0, 0
.text