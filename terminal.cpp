//
//  terminal.cpp
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#include <stdarg.h>
#include "terminal.hpp"

void Terminal() {
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

void setcolor(uint8_t col) {
    color = col;
}

void putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    buffer[index] = make_vgaentry(c, color);
}

void putchar(char c) {
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
    else if(c == '\t') {
        column=column+8-(column%8);
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

void cur() {
    unsigned temp;
    
    temp = row*VGA_WIDTH+column;
    
    outb(0x3D4, 14);
    outb(0x3D5, temp>>8);
    outb(0x3D4, 15);
    outb(0x3D5, temp);
}

void clearLine(uint8_t from, uint8_t to) {
    for (uint16_t i = VGA_WIDTH*from; i<(VGA_WIDTH*to); i++) {
        buffer[i]=make_vgaentry(' ', color);
    }
}

void clearScreen() {
    clearLine(0,VGA_HEIGHT-1);
    row=0;
    column=0;
    cur();
}

void scrollUp(int64_t lineNum) {
    clearLine(0,lineNum-1);
    for (uint16_t i = 0; i<VGA_WIDTH*(VGA_HEIGHT-1); i++) {
        buffer[i]=buffer[i+VGA_WIDTH*lineNum];
    }
    clearLine(VGA_HEIGHT-1-lineNum,VGA_HEIGHT-1);
    if ((row-lineNum) < 0) {
        row=0;
        column=0;
    }
    else {
        row-=lineNum;
    }
    cur();
}

void newLineCheck() {
    if (row>=VGA_HEIGHT-1) {
        scrollUp(1);
    }
}
