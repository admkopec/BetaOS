//
//  memcpy.s
//  Libc
//
//  Created by Adam Kopeć on 11/15/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#define __ASSEMBLY__
#include <i386/asm.h>

ENTRY(memcpy_)
movq    %rdi, %rax             /* return destination */
movq    %rdx,%rcx
shrq    $3,%rcx                /* copy by 64-bit words */
cld                            /* copy forwards */
rep
movsq
movq    %rdx,%rcx
andq    $7,%rcx                /* any bytes left? */
rep
movsb
ret

