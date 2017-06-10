//
//  command.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/15/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <kernel/command.h>
#include <version.h>
#include <time.h>
#include <arch.h>
#include <i386/asm.h>
#include <sys/cdefs.h>
#include "misc_protos.h"

int num = 0;

Command_t command[MAXCOMMANDS];

extern void
change_color(uint32_t foreground, uint32_t background);
extern void
clear_screen();

/* Commands prototypes */

void help();
void version();
void time_(int argc, char* argv[]);

void addCommand(char* name, __unused char* desc, void (*run)(int argc, char* argv[])) {
    for (int i=0; i<num; i++) {
        if (command[i].name==name) {
            error("Failed to add command. Reason: The command's name's been already taken");
            return;
        }
    }
    command[num].name=name;
    if (command[num].name==NULL || command[num].name!=name) {
        error("Failed to add command. Reason: name=NULL");
        return;
    }
    kprintf("");
    command[num].desc=desc;
    if (command[num].desc==NULL || command[num].desc!=desc) {
        error("Failed to add command. Reason: desc=NULL");
        return;
    }
    command[num].run=run;
    if (command[num].run==NULL || command[num].run!=run) {
        error("Failed to add command. Reason: run=NULL");
        return;
    }
    num++;
}

void CommandInit() {
    addCommand("help",          "Help command",                                             help);
    addCommand("version",       "Displays version of BetaOS",                               version);
    addCommand("reboot",        "Reboots the computer",                                     reboot);
    addCommand("clear",         "Clears the screen",                                        clear_screen);
    addCommand("time",          "Displays the actual time",                                 time_);
    addCommand("shutdown",      "Shut downs the computer",                                  shutdown);
    addCommand("stop",          "Shut downs the computer",                                  shutdown);
}

void findcommand() {
    char comm[256];
    int argc = 0;
    char *argv[16];
    gets(comm);
    char* p = comm;
    bool arg = false;
    for (; ;) {
        char ch = *p;
        if (!ch) {
            break;
        }
        
        bool Space = ch == ' ' || ch == '\t';
        if (arg) {
            if (Space) {
                *p = '\0';
                arg = false;
            }
        } else {
            if (!Space) {
                if (argc < 16) {
                    argv[argc] = p;
                    argc++;
                }
                arg = true;
            }
        }
        p++;
    }
    for (int i=0; i<num; i++) {
        if (strcmp(argv[0], command[i].name)) {
            command[i].run(argc, argv);
            return;
        }
    }
    kprintf("Command not found!\n");
}



void help() {
    clear_screen();
    kprintf("======================================HELP======================================\n");
    kprintf("1. Command list\n");
    kprintf("2. Basic information about the OS\n");
    kprintf("3. Exit help\n");
    char menuentry=getchar();
    if (menuentry=='1') {
        clear_screen();
        for (int i=0; i<num; i++) {
            kprintf("Command name: %s\nCommand description: %s\n\n", command[i].name, command[i].desc);
        }
            
        if (getchar()=='\n') {
            help();
            return;
        }
    } else if(menuentry=='2') {
        clear_screen();
        version();
        if (getchar()=='\n') {
            help();
            return;
        }
    } else if(menuentry=='3') {
        clear_screen();
        return;
    }
}
#include <platform/platform.h>
#include <platform/boot.h>
#include <i386/cpuid.h>
#include <i386/cpu_data.h>
void version() {
    kprintf("%s %d.%d %s\n%s\nBuild Number %s\n", OS_NAME, VERSION_MAJOR, VESRION_MINOR, BUILD_TYPE, COPYRIGHT, BUILD_NUMBER);
    
    kprintf("CPU Vendor %s\n", cpuid_info()->cpuid_vendor);
    kprintf("CPU %s\n", cpuid_info()->cpuid_brand_string);
    kprintf("Memory %d GB\n", Platform_state.bootArgs->PhysicalMemorySize/GB);
}

#include <i386/rtclock.h>
void time_(int argc, char* argv[]) {
    gettime();
    if (argc == 2) {
        if(strcmp(argv[1], "long")) {
            kprintf("%d:%02d:%02d %s\n", hour, minute, second, pmam);
            kprintf("%s, %s %d, %d\n", dayofweeklong, monthl, day, year);
        } else if (strcmp(argv[1], "absolute")) {
            kprintf("Absolute time is: %d\n", time());
            kprintf("Mach Absolute time is: %d\n", mach_absolute_time());
        } else {
            goto usage;
        }
    } else if (argc == 1) {
        kprintf("%s %d:%02d %s\n", dayofweekshort, hour, minute, pmam);
    } else {
    usage:
        kprintf("Usage: time [long | absolute]\n");
    }
}
