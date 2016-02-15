//
//  putchar.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>

#if defined(__is_betaos_kernel)
#include <kernel/tty.h>
#endif

int putchar(int ic)
{
#if defined(__is_betaos_kernel)
    char c = (char) ic;
    terminal_write(&c, sizeof(c));
#else
#endif
	return ic;
}
