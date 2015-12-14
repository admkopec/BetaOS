#
#  crti.s
#  OS
#
#  Created by Adam Kopeć on 12/11/15.
#  Copyright © 2015 Adam Kopeć. All rights reserved.
#

.section .init
.global _init
.type _init, @function
_init:
	push %ebp
	movl %esp, %ebp
	/* gcc will nicely put the contents of crtbegin.o's .init section here. */

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %ebp
	movl %esp, %ebp
	/* gcc will nicely put the contents of crtbegin.o's .fini section here. */
