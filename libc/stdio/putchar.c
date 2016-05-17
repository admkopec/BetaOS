//
//  putchar.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <kernel/tty.h>

int putchar(int ic)
{
    if ((ic<=0x7F&&ic>=0x20)||ic=='\n'||ic=='\t'||ic=='\b'||ic=='\r') {
        char c = (char) ic;
        terminal_putchar(c);
    }
	return ic;
}
