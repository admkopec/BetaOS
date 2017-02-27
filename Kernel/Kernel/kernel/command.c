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
extern void
scroll_up();

/* Commands prototypes */

void help();
void version();
void time_full();
void time_short();
void time_absolute();
void pciTest();
void get_platformstate(void);
void testassert() {
    assert(1==0);
}
void testscroll() {
    scroll_up();
}
void inter() {
    asm("int $0x20");
}

void addCommand(char* name, __unused char* desc, void (*run)(void)) {
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
    addCommand("time",          "Displays the actual time",                                 time_short);
    addCommand("time long",     "Displays the actual time in long version",                 time_full);
    addCommand("time absolute", "Displays the actual time in seconds since 1 January 1970", time_absolute);
    addCommand("shut down",     "Shut downs the computer",                                  shutdown);
    addCommand("pciTest",       "Tests PCI",                                                pciTest);
    addCommand("assert",        "Assert",                                                   testassert);
    addCommand("Platform",      "Prints Platform State boot args",                          get_platformstate);
    addCommand("scroll up",     "Tests if scroll up doesn't crash CPU",                     testscroll);
    addCommand("int",           "Sends Interrput $0x20 to CPU",                             inter);
}

void findcommand() {
    char comm[256];
    gets(comm);
    for (int i=0; i<num; i++) {
        if (strcmp(comm, command[i].name)) {
            command[i].run();
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
    for (; ;) {
        char menuentry=getchar();
        if (menuentry=='1') {
            clear_screen();
            for (int i=0; i<num; i++) {
                kprintf("Command name: %s\nCommand description: %s\n\n", command[i].name, command[i].desc);
            }
            for (; ;) {
                if (getchar()=='\n') {
                    help();
                    return;
                }
            }
        } else if(menuentry=='2') {
            clear_screen();
            version();
            for (; ;) {
                if (getchar()=='\n') {
                    help();
                    return;
                }
            }
        } else if(menuentry=='3') {
            clear_screen();
            return;
        }
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

void get_platformstate(void) {
    kprintf("revision      0x%X\n",     Platform_state.bootArgs->Revision);
    kprintf("version       0x%X\n",     Platform_state.bootArgs->Version);
    kprintf("command line  %s\n",       Platform_state.bootArgs->CommandLine);
    kprintf("memory map    0x%X\n",     Platform_state.bootArgs->MemoryMap);
    kprintf("memory map sz 0x%X\n",     Platform_state.bootArgs->MemoryMapSize);
    kprintf("kaddr         0x%X\n",     Platform_state.bootArgs->kaddr);
    kprintf("ksize         0x%X\n",     Platform_state.bootArgs->ksize);
    kprintf("bootargs: %p, &ksize: %p &kaddr: %p\n",
                                        Platform_state.bootArgs,
                                       &Platform_state.bootArgs->ksize,
                                       &Platform_state.bootArgs->kaddr);
    kprintf("SMBIOS mem sz 0x%llx\n",   Platform_state.bootArgs->PhysicalMemorySize);
    kprintf("EFI mode      %d\n",       Platform_state.bootArgs->efiMode);
    kprintf("Video BAR     0x%X\n",     Platform_state.bootArgs->Video.v_baseAddr);
    kprintf("Video Display %d\n",       Platform_state.bootArgs->Video.v_display);
    kprintf("Video rowbyte %d\n",       Platform_state.bootArgs->Video.v_rowBytes);
    kprintf("Video width   %d\n",       Platform_state.bootArgs->Video.v_width);
    kprintf("Video height  %d\n",       Platform_state.bootArgs->Video.v_height);
    kprintf("Video depth   %d\n",       Platform_state.bootArgs->Video.v_depth);
}

void time_full() {
    gettime();
    kprintf("%d:%02d:%02d %s\n", hour, minute, second, pmam);
    kprintf("%s, %s %d, %d\n", dayofweeklong, monthl, day, year);
}

void time_short() {
    gettime();
    kprintf("%s %d:%02d %s\n", dayofweekshort, hour, minute, pmam);
}
#include <i386/rtclock.h>
void time_absolute() {
    kprintf("Absolute time is: %d\n", time());
    kprintf("Mach Absolute time is: %d\n", mach_absolute_time());
}
#include <i386/pci.h>
void pciTest() {
    kprintf("Should I dump? (Y/N) //For now only dump works ");
    char yn;
    for (;;) {
        yn = getchar();
        if (yn=='Y'||yn=='N'||yn=='y'||yn=='n') {
            break;
        }
    }
    if (yn=='Y'||yn=='y') {
        kprintf("%c\n", yn);
        pcidump();
    }
//#include <modules/SATAController.h>
    //testSATA();   // Crashes CPU
}
