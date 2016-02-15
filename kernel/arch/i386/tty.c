//
//  tty.c
//  BetaOS
//
//  Created by Adam Kopeć on 9/26/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/interrupts.h>

#include <PS2Controller.h>

uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_WHITE, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void clearLine(uint8_t from, uint8_t to) {
    for (uint16_t i = VGA_WIDTH*from; i<(VGA_WIDTH*to); i++) {
        terminal_buffer[i]=make_vgaentry(' ', terminal_color);
    }
}

void cur() {
    unsigned short position=(terminal_row*VGA_WIDTH) + terminal_column;

    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
  	outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

void clearScreen() {
    clearLine(0,VGA_HEIGHT-1);
    terminal_row=0;
    terminal_column=0;
    cur();
}

void scrollUp(int64_t lineNum) {
    clearLine(0,lineNum-1);
    for (uint16_t i = 0; i<VGA_WIDTH*(VGA_HEIGHT-1); i++) {
        terminal_buffer[i]=terminal_buffer[i+VGA_WIDTH*lineNum];
    }
    clearLine(VGA_HEIGHT-1-lineNum,VGA_HEIGHT-1);
    if ((terminal_row-lineNum) < 0) {
        terminal_row=0;
        terminal_column=0;
    }
    else {
        terminal_row-=lineNum;
    }
    cur();
}

void newLineCheck() {
    if (terminal_row>=VGA_HEIGHT-1) {
        scrollUp(1);
    }
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	if (c=='\n') {
		terminal_column=0;
		terminal_row++;
	}
	else if (c=='\b') {
		if (terminal_row>0||terminal_row==0) {
			terminal_column--;
			if (terminal_column==0&&terminal_row!=0) {
				terminal_column=VGA_WIDTH;
				terminal_row--;
			}
			terminal_buffer[(terminal_row*VGA_WIDTH+terminal_column)]=make_vgaentry(' ', terminal_color);
		}
	}
	else if (c=='\t') {
		terminal_column=terminal_column+8-(terminal_column%8);
	}
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if ( ++terminal_column == VGA_WIDTH ) {
			terminal_column = 0;
		}
	}
    cur();
    newLineCheck();
}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
