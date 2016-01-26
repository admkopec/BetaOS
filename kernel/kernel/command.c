//
//  command.c
//  OS
//
//  Created by Adam Kopeć on 12/15/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <version.h>
#include <command.h>
#include <kernel/tty.h>
#include <kernel/interrupts.h>
#include <kernel/arch/arch.h>
#include <kernel/arch/pic.h>

int num = 0;

/* Commands prototypes */

void help();
void version();
void time_full();
void time_short();

void addCommand(char* name, char* desc, void (*run)(void)) {
    command[num].name=name;
    command[num].desc=desc;
    command[num].run=run;
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
}

void findcommand() {
    read();
    for (int i=0; i<100; i++) {
        if (streql(buffstr, command[i].name)) {
            command[i].run();
            return;
        }
    }
    printf("Command not found!\n");
}



void help() {
    clearScreen();
    printf("==================HELP=================\n");
    printf("1. Command list\n");
    printf("2. Basic information about the OS\n");
    printf("3. Exit help\n");
    for (; ;) {
        read();
        if (streql(buffstr,"1")) {
            clearScreen();
            for (int i=0; i<num; i++) {
                printf("Command name: %s\nCommand description: %s\n\n", command[i].name, command[i].desc);
            }
            for (; ;) {
                read();
                if (streql(buffstr, "")) {
                    help();
                    return;
                }
            }
        } else if(streql(buffstr, "2")) {
            clearScreen();
            version();
            for (; ;) {
                read();
                if (streql(buffstr, "")) {
                    help();
                    return;
                }
            }
        } else if(streql(buffstr, "3")) {
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



////////////////////////   Keyboard Read()   ////////////////////////

void read() {
    for (int i=0; i<214; i++) {
        buffstr[i]=buffstr[214];
    }
    unsigned char i = 0;
    unsigned char reading=1;
    while (reading) {
        if (inb(0x64)&0x1) {
        switch (inb(0x60)) {
                case 2:
                    printf("1");
                    buffstr[i]='1';
                    i++;
                    break;
                case 3:
                    printf("2");
                    buffstr[i] = '2';
                    i++;
                    break;
                case 4:
                    printf("3");
                    buffstr[i] = '3';
                    i++;
                    break;
                case 5:
                    printf("4");
                    buffstr[i] = '4';
                    i++;
                    break;
                case 6:
                    printf("5");
                    buffstr[i] = '5';
                    i++;
                    break;
                case 7:
                    printf("6");
                    buffstr[i] = '6';
                    i++;
                    break;
                    /*case 8:
                     printf("7");
                     buffstr[i] = '7';           This bit is used also for mouse input.
                     i++;
                     break;*/
                case 9:
                    printf("8");
                    buffstr[i] = '8';
                    i++;
                    break;
                case 10:
                    printf("9");
                    buffstr[i] = '9';
                    i++;
                    break;
                case 11:
                    printf("0");
                    buffstr[i] = '0';
                    i++;
                    break;
                case 12:
                    printf("-");
                    buffstr[i] = '-';
                    i++;
                    break;
                case 13:
                    printf("=");
                    buffstr[i] = '=';
                    i++;
                    break;
                case 14:
                    printf("\b");
                    i--;
                    buffstr[i] = 0;
                    break;
                case 15:
                    printf("\t");             //Tab button
                    buffstr[i] = '\t';
                    i++;
                    break;
                case 16:
                    printf("q");
                    buffstr[i] = 'q';
                    i++;
                    break;
                case 17:
                    printf("w");
                    buffstr[i] = 'w';
                    i++;
                    break;
                case 18:
                    printf("e");
                    buffstr[i] = 'e';
                    i++;
                    break;
                case 19:
                    printf("r");
                    buffstr[i] = 'r';
                    i++;
                    break;
                case 20:
                    printf("t");
                    buffstr[i] = 't';
                    i++;
                    break;
                case 21:
                    printf("y");
                    buffstr[i] = 'y';
                    i++;
                    break;
                case 22:
                    printf("u");
                    buffstr[i] = 'u';
                    i++;
                    break;
                case 23:
                    printf("i");
                    buffstr[i] = 'i';
                    i++;
                    break;
                case 24:
                    printf("o");
                    buffstr[i] = 'o';
                    i++;
                    break;
                case 25:
                    printf("p");
                    buffstr[i] = 'p';
                    i++;
                    break;
                case 26:
                    printf("[");
                    buffstr[i] = '[';
                    i++;
                    break;
                case 27:
                    printf("]");
                    buffstr[i] = ']';
                    i++;
                    break;
                case 28:
                    printf("\n");
                    /*buffstr[i] = '\n';
                     i++;*/
                    reading = 0;
                    break;
                    /*  case 29:
                     printch('q');           Left Control
                     buffstr[i] = 'q';
                     i++;
                     break;*/
                case 30:
                    printf("a");
                    buffstr[i] = 'a';
                    i++;
                    break;
                case 31:
                    printf("s");
                    buffstr[i] = 's';
                    i++;
                    break;
                case 32:
                    printf("d");
                    buffstr[i] = 'd';
                    i++;
                    break;
                case 33:
                    printf("f");
                    buffstr[i] = 'f';
                    i++;
                    break;
                case 34:
                    printf("g");
                    buffstr[i] = 'g';
                    i++;
                    break;
                case 35:
                    printf("h");
                    buffstr[i] = 'h';
                    i++;
                    break;
                case 36:
                    printf("j");
                    buffstr[i] = 'j';
                    i++;
                    break;
                case 37:
                    printf("k");
                    buffstr[i] = 'k';
                    i++;
                    break;
                case 38:
                    printf("l");
                    buffstr[i] = 'l';
                    i++;
                    break;
                case 39:
                    printf(";");
                    buffstr[i] = ';';
                    i++;
                    break;
                case 40:
                    printf("'");               //   Single quote (')
                    buffstr[i] = (char)39;
                    i++;
                    break;
                case 41:
                    printf("`");               // Back tick (`)
                    buffstr[i] = (char)96;
                    i++;
                    break;
                    /* case 42:                                 Left shift
                     printch('q');
                     buffstr[i] = 'q';
                     i++;
                     break;
                     case 43:                                 \ (< for somekeyboards)
                     printch((char)92);
                     buffstr[i] = 'q';
                     i++;
                     break;*/
                case 44:
                    printf("z");
                    buffstr[i] = 'z';
                    i++;
                    break;
                case 45:
                    printf("x");
                    buffstr[i] = 'x';
                    i++;
                    break;
                case 46:
                    printf("c");
                    buffstr[i] = 'c';
                    i++;
                    break;
                case 47:
                    printf("v");
                    buffstr[i] = 'v';
                    i++;
                    break;
                case 48:
                    printf("b");
                    buffstr[i] = 'b';
                    i++;
                    break;
                case 49:
                    printf("n");
                    buffstr[i] = 'n';
                    i++;
                    break;
                case 50:
                    printf("m");
                    buffstr[i] = 'm';
                    i++;
                    break;
                case 51:
                    printf(",");
                    buffstr[i] = ',';
                    i++;
                    break;
                case 52:
                    printf(".");
                    buffstr[i] = '.';
                    i++;
                    break;
                case 53:
                    printf("/");
                    buffstr[i] = '/';
                    i++;
                    break;
                case 54:
                    printf(".");
                    buffstr[i] = '.';
                    i++;
                    break;
                case 55:
                    printf("/");
                    buffstr[i] = '/';
                    i++;
                    break;
                    /*case 56:
                     printch(' ');           Right shift
                     buffstr[i] = ' ';
                     i++;
                     break;*/
                case 57:
                    printf(" ");
                    buffstr[i] = ' ';
                    i++;
                    break;
                default:
                    break;
            }
        }
    }
}