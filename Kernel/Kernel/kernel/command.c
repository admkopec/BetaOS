//
//  command.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/15/15.
//  Copyright © 2015-2018 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
clear_screen(void);

/* Commands prototypes */

void help(int argc, char* argv[]);
void version(int argc, char* argv[]);
void time_(int argc, char* argv[]);
void set_color(int argc, char* argv[]);
void printLoadedModules(int argc, char* argv[]);
void test_graphics(int argc, char* argv[]);
void test_new_panic(int argc, char* argv[]);
void openFile(int argc, char* argv[]);
void writeFile(int argc, char* argv[]);
void listFiles(int argc, char* argv[]);
void runExec(int argc, char* argv[]);
void tasks(int argc, char* argv[]);
void reboot_(__unused int argc,__unused char* argv[]) {
    reboot_system(false);
}
void shutdown_(__unused int argc, __unused char* argv[]);
void clear_screen_(__unused int argc, __unused char* argv[]) {
    clear_screen();
}

void addCommand(char* name, __unused char* desc, void (*run)(int argc, char* argv[])) {
    for (int i=0; i<num; i++) {
        if (command[i].name==name) {
            printf("Failed to add command. Reason: The command's name's been already taken");
            return;
        }
    }
    command[num].name=name;
    if (command[num].name==NULL || command[num].name!=name) {
        printf("Failed to add command. Reason: name=NULL");
        return;
    }
    printf("");
    command[num].desc=desc;
    if (command[num].desc==NULL || command[num].desc!=desc) {
        printf("Failed to add command. Reason: desc=NULL");
        return;
    }
    command[num].run=run;
    if (command[num].run==NULL || command[num].run!=run) {
        printf("Failed to add command. Reason: run=NULL");
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
    addCommand("setcolor",      "Sets the terminal colors",                                 set_color);
    addCommand("loadedModules", "Displays Loaded Modules",                                  printLoadedModules);
    addCommand("runTasks",      "Runs the tasks queue",                                     tasks);
    addCommand("graphics",      "Test Graphics drawing",                                    test_graphics);
    addCommand("open",          "Opens a File from Disk",                                   openFile);
    addCommand("write",         "Writes a File to Disk",                                    writeFile);
    addCommand("list",          "Lists files and folders",                                  listFiles);
    addCommand("panic",         "Test Graphical Panic",                                     test_new_panic);
    addCommand("run",           "Runs an executable from disk",                             runExec);
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
    bool Escaping = false;
    for (; ;) {
        char ch = *p;
        if (!ch) {
            break;
        }
        bool Escape = ch == '"' || ch == '\'';
        bool Space  = (ch == ' ' || ch == '\t') && !Escaping;
        if (Escape) {
            Escaping = true;
        }
        if (arg) {
            if (Space || (Escape && Escaping)) {
                *p = '\0';
                Escaping = false;
                arg = false;
            }
        } else {
            if (!Space) {
                if (argc < 16) {
                    if (Escaping) {
                        argv[argc] = p+1;
                    } else {
                        argv[argc] = p;
                    }
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
    printf("Command not found!\n");
}



void help(int argc, char* argv[]) {
    clear_screen();
    printf("======================================HELP======================================\n");
    printf("1. Command list\n");
    printf("2. Basic information about the OS\n");
    printf("3. Exit help\n");
    char menuentry=getchar();
    if (menuentry=='1') {
        clear_screen();
        for (int i=0; i<num; i++) {
            printf("Command name: %s\nCommand description: %s\n\n", command[i].name, command[i].desc);
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
