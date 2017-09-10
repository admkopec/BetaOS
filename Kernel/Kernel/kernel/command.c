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
bool isUpArrowKey = false;

Command_t command[MAXCOMMANDS];
char*     commands[256];
uint16_t  LastRunCommand = 0;

extern void
change_color(uint32_t foreground, uint32_t background);
extern void
clear_screen(void);
extern void
PrintLoadedModules(void);

/* Commands prototypes */

void help(int argc, char* argv[]);
void version(int argc, char* argv[]);
void time_(int argc, char* argv[]);
void set_colors(int argc, char* argv[]);
void reboot_(__unused int argc,__unused char* argv[]) {
    reboot(false);
}
void printLoadedModules(__unused int argc,__unused char* argv[]) {
    printf("Loaded Modules: \n");
    PrintLoadedModules();
}
void shutdown_(__unused int argc, __unused char* argv[]) {
    shutdown();
}
void clear_screen_(__unused int argc, __unused char* argv[]) {
    clear_screen();
}

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
    addCommand("reboot",        "Reboots the computer",                                     reboot_);
    addCommand("clear",         "Clears the screen",                                        clear_screen_);
    addCommand("time",          "Displays the actual time",                                 time_);
    addCommand("shutdown",      "Shut downs the computer",                                  shutdown_);
    addCommand("stop",          "Shut downs the computer",                                  shutdown_);
    addCommand("setcolor",      "Sets the terminal colors",                                 set_colors);
    addCommand("loadedModules", "Displays Loaded Modules",                                  printLoadedModules);
}

void findcommand() {
    char comm[256];
    int argc = 0;
    char *argv[16];
    gets(comm);
    if (strcmp(comm, "\n") || strcmp(comm, "") || strcmp(comm, " ")) {
        goto fail;
    }
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
            commands[LastRunCommand] = p;
            if (LastRunCommand != 256) {
                LastRunCommand++;
            } else {
                LastRunCommand = 1;
            }
            command[i].run(argc, argv);
            return;
        }
    }
fail:
    kprintf("Command not found!\n");
}



void help(int argc, char* argv[]) {
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
            help(argc, argv);
            return;
        }
    } else if(menuentry=='2') {
        clear_screen();
        version(argc, argv);
        if (getchar()=='\n') {
            help(argc, argv);
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
void version(__unused int argc, __unused char* argv[]) {
    kprintf("%s %d.%d", OS_NAME, VERSION_MAJOR, VESRION_MINOR);
    if (VERSION_XMINOR > 0) {
        kprintf(".%d", VERSION_XMINOR);
    }
    kprintf(" %s(%X)\n%s\n", BUILD_TYPE, BUILD_NUMBER, COPYRIGHT);
    
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

uint8_t getNum(char* reds) {
    uint8_t red = 0x00;
    for (int i = 0; i < 2; i++) {
        if (reds[i] >= '0' && reds[i] <= '9') {
            red = (reds[i] - '0') * (i == 0 ? 16 : 0);
        } else {
            switch (reds[i]) {
                case 'A':
                    red = 10 * (i == 0 ? 16 : 0);
                    break;
                case 'B':
                    red = 11 * (i == 0 ? 16 : 0);
                case 'C':
                    red = 12 * (i == 0 ? 16 : 0);
                case 'D':
                    red = 13 * (i == 0 ? 16 : 0);
                case 'E':
                    red = 14 * (i == 0 ? 16 : 0);
                case 'F':
                    red = 15 * (i == 0 ? 16 : 0);
                default:
                    break;
            }
        }
    }
    return red;
}
// Needs a fix
void set_colors(int argc, char* argv[]) {
    if (argc == 7) {
        char* fred   = argv[1];
        char* fgreen = argv[2];
        char* fblue  = argv[3];
        char* bred   = argv[4];
        char* bgreen = argv[5];
        char* bblue  = argv[6];
        uint8_t red, green, blue;
        
        red     = getNum(fred);
        green   = getNum(fgreen);
        blue    = getNum(fblue);
        uint32_t foreground = 0x00;
        foreground += red   * (16 * 2);
        foreground += green * (16 * 1);
        foreground += blue  * (16 * 0);
        
        red     = getNum(bred);
        green   = getNum(bgreen);
        blue    = getNum(bblue);
        uint32_t background = 0x00;
        background += red   * (16 * 2);
        background += green * (16 * 1);
        background += blue  * (16 * 0);
        
        if (foreground == background) {
            kprintf("Foreground can't be the same as background!\n");
            return ;
        }
        
        change_color(foreground, background);
    } else {
        kprintf("Usage: setcolor RR GG BB RR GG BB\n");
        kprintf("                (Foregr) (Backgr)\n");
    }
}
