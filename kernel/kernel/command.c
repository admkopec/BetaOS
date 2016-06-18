//
//  command.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/15/15.
//  Copyright © 2015-2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <kernel/command.h>
#include <version.h>
#include <time.h>
#include <kernel/tty.h>
#include <arch.h>
#include <i386/asm.h>
#include <sys/cdefs.h>

int num = 0;

/* Commands prototypes */

void help();
void version();
void time_full();
void time_short();
void pciTest();


void addCommand(char* name, char* desc, void (*run)(void)) {
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
    //command[num].desc=desc;
    //if (command[num].desc==NULL || command[num].desc!=desc) {
    //    error("Failed to add command. Reason: desc=NULL");
    //    return;
    //}
    command[num].run=run;
    if (command[num].run==NULL || command[num].run!=run) {
        error("Failed to add command. Reason: run=NULL");
        return;
    }
    num++;
}

void CommandInit() {
    addCommand("help",      "Help command",                             help);
    addCommand("version",   "Displays version of BetaOS",               version);
    addCommand("reboot",    "Reboots the computer",                     reboot);
    addCommand("clear",     "Clears the screen",                        clearScreen);
    addCommand("time",      "Displays the actual time",                 time_short);
    addCommand("time long", "Displays the actual time in long version", time_full);
    addCommand("shut down", "Shut downs the computer",                  shutdown);
    addCommand("pciTest",   "Tests PCI",                                pciTest);
}

void findcommand() {
    char comm[256];
    gets(comm);
    for (int i=0; i<MAXCOMMANDS; i++) {
        if (strcmp(comm, command[i].name)) {
            command[i].run();
            return;
        }
    }
    printf("Command not found!\n");
}



void help() {
    clearScreen();
    printf("======================================HELP======================================");
    printf("1. Command list\n");
    printf("2. Basic information about the OS\n");
    printf("3. Exit help\n");
    for (; ;) {
        char menuentry=getchar();
        if (menuentry=='1') {
            clearScreen();
            for (int i=0; i<num; i++) {
                printf("Command name: %s"/*\nCommand description: %s*/"\n\n", command[i].name/*, command[i].desc*/);
            }
            for (; ;) {
                if (getchar()=='\n') {
                    help();
                    return;
                }
            }
        } else if(menuentry=='2') {
            clearScreen();
            version();
            for (; ;) {
                if (getchar()=='\n') {
                    help();
                    return;
                }
            }
        } else if(menuentry=='3') {
            clearScreen();
            return;
        }
    }
}

void version() {
    printf(VERSION_NAME);
    printf(" ");
    printf(VERSION_MAJOR);
    printf(".");
    printf(VESRION_MINOR);
    printf(" ");
    printf(VERSION_COPYRIGHT);
    printf("\n");
    printf("Build Number ");
    printf(VERSION_BUILD);
    printf("\n");
    
    #include <i386/cpuid.h>
    
    uint32_t reg[4];
    char CPU_vendor[12];
    char CPU_name[48];
    do_cpuid(0, reg);
    memcpy(CPU_vendor,   (char *)&reg[ebx], sizeof(4));
    memcpy(CPU_vendor+4, (char *)&reg[edx], sizeof(4));
    memcpy(CPU_vendor+8, (char *)&reg[ecx], sizeof(4));
    printf("CPU Vendor %s\n", CPU_vendor);
    
    do_cpuid(0x80000002, reg);
    memcpy(CPU_name+0,   (char *)&reg[eax], sizeof(4));
    memcpy(CPU_name+4,   (char *)&reg[ebx], sizeof(4));
    memcpy(CPU_name+8,   (char *)&reg[ecx], sizeof(4));
    memcpy(CPU_name+12,  (char *)&reg[edx], sizeof(4));
    
    do_cpuid(0x80000003, reg);
    memcpy(CPU_name+16,   (char *)&reg[eax], sizeof(4));
    memcpy(CPU_name+20,   (char *)&reg[ebx], sizeof(4));
    memcpy(CPU_name+24,   (char *)&reg[ecx], sizeof(4));
    memcpy(CPU_name+28,   (char *)&reg[edx], sizeof(4));
    
    do_cpuid(0x80000004, reg);
    memcpy(CPU_name+32,   (char *)&reg[eax], sizeof(4));
    memcpy(CPU_name+36,   (char *)&reg[ebx], sizeof(4));
    memcpy(CPU_name+40,   (char *)&reg[ecx], sizeof(4));
    memcpy(CPU_name+44,   (char *)&reg[edx], sizeof(4));

    int j=0;
    for (j=0; ; j++) {
        if (CPU_name[j]!=' ') {
            break;
        }
    }
    for (int i=j; i<48; i++) {
        CPU_name[i-j]=CPU_name[i];
    }

    printf("CPU %s\n", CPU_name);
}

void time_full() {
    gettime();
    printf("%d:%s%d:%s%d %s\n", hour, zerom, minute, zeros, second, pmam);
    printf("%s, %s %d, %d\n", dayofweeklong, monthl, day, year);
}

void time_short() {
    gettime();
    printf("%s %d:%s%d %s\n", dayofweekshort, hour, zerom, minute, pmam);
}
#include <i386/pci.h>
void pciTest() {
    printf("Should I dump? (Y/N) //For now only dump works ");
    char yn;
    for (;;) {
        yn = getchar();
        if (yn=='Y'||yn=='N'||yn=='y'||yn=='n') {
            break;
        }
    }
    if (yn=='Y') {
        printf("%c\n", yn);
        pcidump();
    }
}
