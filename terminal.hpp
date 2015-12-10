//
//  terminal.hpp
//  OS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//
#pragma once
#ifndef terminal_hpp
#define terminal_hpp

#include <stdbool.h> /* C doesn't have booleans by default. */
#include <stddef.h>
#include <stdint.h>
#include <interrupts.hpp>

/* Hardware text mode color constants. */
enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

    static const signed int VGA_WIDTH = 80;
    static const signed int VGA_HEIGHT = 25;
    
    signed int row;
    signed int column;
    
    uint8_t color;
    uint16_t* buffer;
    char buffstr[VGA_WIDTH];
    
    void setcolor(uint8_t color);
    
    void putchar(char c);
    
    void read();
    
    void putentryat(char c, uint8_t color, size_t x, size_t y);
    
    void clearLine(uint8_t from, uint8_t to);
    
    void scrollUp(int64_t lineNum);

    void Terminal();
    
    void getcommand();
    
    void cur();
    
    void clearScreen();
    
    void newLineCheck();

#endif /* terminal_hpp */
