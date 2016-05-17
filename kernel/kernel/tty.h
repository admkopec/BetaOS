//
//  tty.h
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

size_t terminal_row;
size_t terminal_column;

void terminal_initialize(void);
void terminal_putchar(char c);
void clearScreen();
void cur();
void newLineCheck();

#endif /* tty_h */
