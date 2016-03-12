//
//  asm.h
//  BetaOS
//
//  Created by Adam Kopeć on 3/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef asm_h
#define asm_h

#if !defined(_ALIGN_TEXT) && !defined(_KERNEL)
# ifdef _STANDALONE
#  define _ALIGN_TEXT .align 1
# elif defined __ELF__
#  define _ALIGN_TEXT .align 16
# else
#  define _ALIGN_TEXT .align 4
# endif
#else
# define _ALIGN_TEXT .align 16      // Just untill not propearly implemented
#endif

#define _ENTRY(x) \
    .text; _ALIGN_TEXT; .globl x; .type x,@function; x:
#define _LABEL(x) \
    .globl x; x:

/* #define ENTRY(y)    _ENTRY(_C_LABEL(y)); _PROF_PROLOGUE */   // To be implemented in future


#endif /* asm_h */
