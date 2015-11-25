//
//  terminal.cpp
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include "terminal.hpp"
#include "version.h"

Terminal::Terminal() {
    row = 0;
    column = 0;
    //color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    color = make_color(COLOR_WHITE, COLOR_BLACK);
    //color = make_color(COLOR_BLUE, COLOR_GREEN);
    buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            buffer[index] = make_vgaentry(' ', color);
        }
    }
}

size_t Terminal::strlen(const char* str) {
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}

bool Terminal::streql(const char* str1, const char* str2) {
    if (strlen(str1)!=strlen(str2)) {
        return false;
    }
    else {
        for (uint8_t i=0; i<=strlen(str1); i++) {
            if (str1[i] != str2[i]) {
                return false;
            }
        }
    }
    return true;
}

void Terminal::setcolor(uint8_t col) {
    color = col;
}

void Terminal::putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    buffer[index] = make_vgaentry(c, color);
}

void Terminal::putchar(char c) {
    if (c == '\n') {
        column = 0;
        row++;
    }
    else if (c == '\b') {
        if (row>0||row==0) {
            column--;
            if(column==-1&&row!=0) {
                column=VGA_WIDTH;
                row--;
            }
            buffer[(row*VGA_WIDTH+column)]=make_vgaentry(' ', color);
        }
    }
    else {
        putentryat(c, color, column, row);
        if (++column == VGA_WIDTH) {
            column = 0;
            if (++row == VGA_HEIGHT) {
                row = 0;
            }
        }
    }
    cur();
    newLineCheck();
}

void Terminal::printf(const char* data) {
    size_t datalen = strlen(data);
    for (size_t i = 0; i < datalen; i++)
        putchar(data[i]);
}

void Terminal::cur() {
    unsigned temp;
    
    temp = row*VGA_WIDTH+column;
    
    interrupts.outb(0x3D4, 14);
    interrupts.outb(0x3D5, temp>>8);
    interrupts.outb(0x3D4, 15);
    interrupts.outb(0x3D5, temp);
}

void Terminal::clearLine(uint8_t from, uint8_t to) {
    for (uint16_t i = VGA_WIDTH*from; i<(VGA_WIDTH*to); i++) {
        buffer[i]=make_vgaentry(' ', color);
    }
}

void Terminal::clearScreen() {
    clearLine(0,VGA_HEIGHT-1);
    row=0;
    column=0;
    cur();
}

void Terminal::scrollUp(uint64_t lineNum) {
    uint16_t i = 0;
    clearLine(0,lineNum-1);
    for (i; i<VGA_WIDTH*(VGA_HEIGHT-1); i++) {
        buffer[i]=buffer[i+VGA_WIDTH*lineNum];
    }
    clearLine(VGA_HEIGHT-1-lineNum,VGA_HEIGHT-1);
    if ((row-lineNum)<0) {
        row=0;
        column=0;
    }
    else {
        row-=lineNum;
    }
    cur();
}

void Terminal::newLineCheck() {
    if (row>=VGA_HEIGHT-1) {
        scrollUp(1);
    }
}

void Terminal::getcommand() {
    read();
    /*for (long long i=0; i<sizeof(commands); i++) {
     
    }*/
    if (streql(buffstr, "clear")) {
        clearScreen();
    }
    else if (streql(buffstr, "shut down")) {
        printf("Not supported in this version! Working ACPI is needed\n");
    }
    else if (streql(buffstr, "reboot")) {
        printf("Not supported in this version!\n");
    }
    else if (streql(buffstr, "version")) {
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
    else if (streql(buffstr, "dev test 3 digit")) {
        int a = 345;
        //a = a/100;
        //a = (int)(a < 0 ? (a - 0.5) : (a + 0.5));
        char b[3];
        b[1] = (char)(a/100 < 0 ? (a/100 - 0.5) : (a/100 + 0.5))+48;                                                    /// Works
        b[2] = (char)(a/10 < 0 ? (a/10 - 0.5) : (a/10 + 0.5))-((a/100 < 0 ? (a/100 - 0.5) : (a/100 + 0.5))*10)+48;      /// Doesn't work
        putchar(b[1]);
        putchar(b[2]);
        printf("\n");
    }
    else if (streql(buffstr, "dev test 1 digit")) {
        int a = 8;
        char b = (char)a+48;                /// Works
        putchar(b);
        printf("\n");
    }
    else {
        printf("Command not found!\n");
    }
}

void Terminal::read() {
    for (int i=0; i<79; i++) {
        buffstr[i]=buffstr[80];
    }
    uint8_t i = 0;
    uint8_t reading=1;
    while (reading) {
        if (interrupts.inb(0x64)&0x1) {
            switch (interrupts.inb(0x60)) {
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
                    /* case 15:
                     printch('\t');          Tab button
                     buffstr[i] = '\t';
                     i++;
                     break;*/
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
                    putchar('\n');
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
                    putchar('a');
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
                    putchar((char)39);               //   Single quote (')
                    buffstr[i] = (char)44;
                    i++;
                    break;
                case 41:
                    putchar((char)96);               // Back tick (`)
                    buffstr[i] = (char)44;
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

