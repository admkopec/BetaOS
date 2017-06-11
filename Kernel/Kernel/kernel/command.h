//
//  command.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/11/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _KERNEL_COMMAND_H
#define _KERNEL_COMMAND_H

#define MAXCOMMANDS 100

typedef struct {
    char* name;
    char* desc;
    void (*run)(int argc, char* argv[]);
} Command_t;

extern bool returnn;

void CommandInit(void);
void findcommand(void);

#endif /* command_h */
