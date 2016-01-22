//
//  command.h
//  OS
//
//  Created by Adam Kopeć on 12/11/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _KERNEL_COMMAND_H
#define _KERNEL_COMMAND_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* name;
    char* desc;
    void (*run)(void);
} Commands_t;

Commands_t command[100];

void CommandsInit();
void findcommand();

////// Temporary keyboard read() //////
char buffstr[215];
void read();

#endif /* command_h */