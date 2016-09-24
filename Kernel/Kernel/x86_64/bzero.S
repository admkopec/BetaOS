//
//  bzero.s
//  BetaOS
//
//  Created by Adam Kopeć on 6/27/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <i386/asm.h>

/*
 * void bzero(char * addr, size_t length)
 */
Entry(blkclr)
ENTRY2(bzero,__bzero)
	movq	%rsi,%rcx
	xorq	%rax,%rax
	shrq	$3,%rcx
	cld
	rep
	stosq
	movq	%rsi,%rcx
	andq	$7,%rcx
	rep
	stosb
	ret
