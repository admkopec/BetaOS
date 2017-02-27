//
//  renderer.c
//  BetaOS
//
//  Created by Adam Kopeć on 9/12/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <stdint.h>
#include <platform/platform.h>
#include <sys/cdefs.h>
#include "font.h"
#include "../misc_protos.h"

#define VM_MIN_KERNEL_ADDRESS		((vm_offset_t) 0xFFFFFF8000000000UL)
#define FRAMEBUFFER_ALIAS           (VM_MIN_KERNEL_ADDRESS + 0x5000)

static uint32_t color_foreground = 0x00FFFFFF;
static uint32_t color_background = 0x00000000;

char painted_chars[1000][1000];
unsigned long painted_chars_row    = 0;
unsigned long painted_chars_column = 0;

void putc(int ch);

uint32_t row = 0;
uint32_t column = 0;

#define REN_MAX_DEPTH	32
static unsigned char rendered_char[ISO_CHAR_HEIGHT * ((REN_MAX_DEPTH / 8) * ISO_CHAR_WIDTH)];

void
paint_char(unsigned int x, unsigned int y, unsigned char ch);

void
change_color(uint32_t foreground, uint32_t background) {
    color_foreground = foreground;
    color_background = background;
}
uint32_t line = 0;
uint32_t col  = 0;
void
clear_screen() {
    for (/*uint32_t */line = 0; line + ISO_CHAR_HEIGHT < Platform_state.video.v_height; line+=ISO_CHAR_HEIGHT) {
        for (/*uint32_t */col = 0; col < Platform_state.video.v_width; col+=ISO_CHAR_WIDTH) {
            paint_char(col, line, '\0');
        }
    }
    row=0;
    column=0;
}

static void
render_char(unsigned char ch, unsigned char *renderptr, short newdepth) {
    union {
        unsigned char  *charptr;
        unsigned short *shortptr;
        uint32_t       *longptr;
    } current; 	/* current place in rendered font, multiple types. */
    unsigned char *theChar;	/* current char in iso_font */
    int line;
    
    current.charptr = renderptr;
    theChar = iso_font + (ch * ISO_CHAR_HEIGHT);
    
    for (line = 0; line < ISO_CHAR_HEIGHT; line++) {
        unsigned char mask = 1;
        do {
            switch (newdepth) {
                case 8:
                    *current.charptr++ = (*theChar & mask) ? 0xFF : 0;
                    break;
                case 16:
                    *current.shortptr++ = (*theChar & mask) ? 0xFFFF : 0;
                    break;
                    
                case 30:
                case 32: 
                    *current.longptr++ = (*theChar & mask) ? 0xFFFFFFFF : 0;
                    break;
            }
            mask <<= 1;
        } while (mask);	/* while the single bit drops to the right */
        theChar++;
    }
}

void
paint_char(unsigned int x, unsigned int y, unsigned char ch) {
    uint32_t *theChar;
    uint32_t *where;
    int       i;
    
    render_char(ch, rendered_char, 32);
    theChar = (uint32_t*)(rendered_char);
    
    where = (uint32_t*)(Platform_state.video.v_baseAddr + (y * Platform_state.video.v_rowBytes) + (x * 4));
    
    for (i = 0; i < ISO_CHAR_HEIGHT; i++) {
        uint32_t *store = where;
        int j;
        for (j = 0; j < 8; j++) {
            uint32_t val = *theChar++;
            val = (color_background & ~val) | (color_foreground & val);
            *store++ = val;
        }
        where = (uint32_t *)(((unsigned char*)where)+Platform_state.video.v_rowBytes);
    }
}

void
scroll_up() {  // Fix clearing painted_chars every time scroll occures
    int i_ = (int)painted_chars_row;
    //painted_chars_row = 0;
    //painted_chars_column = 0;
    //int j = place;
    clear_screen();
    /*for (int i = 0; (i <= j); i++) {
        putc(chars[i]);
    }*/
    /*for (uint32_t i = 0; ((i < painted_chars_row) && (i <= Platform_state.video.v_height/ISO_CHAR_HEIGHT)); i++) {
        for (uint32_t j = 0; j < painted_chars_column; j++) {
            paint_char(j*ISO_CHAR_WIDTH, i*ISO_CHAR_HEIGHT, painted_chars[i][j]);
        }
    }*/
    __unused unsigned long start_row = painted_chars_row - ((Platform_state.video.v_height/ISO_CHAR_HEIGHT)-1);
    for (int i = (int)start_row; i <= i_; i++) {
        //kprintf(painted_chars[i]);
        for (int j = 0; j<200; j++) {
            if (painted_chars[i][j]!='\0') {
                //painted_chars[i-1][j] = 0;
                //paint_char((unsigned int)(j*ISO_CHAR_WIDTH), (unsigned int)((start_row-(Platform_state.video.v_height/ISO_CHAR_HEIGHT))*ISO_CHAR_HEIGHT), painted_chars[i][j]);
                putc(painted_chars[i][j]);
            }
        }
    }
    //column=0;
    //row = (uint32_t)(Platform_state.video.v_height-(2*ISO_CHAR_HEIGHT));
}

void
putc(int ch) {
    painted_chars[painted_chars_row][painted_chars_column] = ch;
    if (ch=='\n'||ch=='\r') {
        painted_chars_row++;
        painted_chars_column=0;
        if (row>=(Platform_state.video.v_height-(2*ISO_CHAR_HEIGHT))) {
            //scroll_up();
            clear_screen();
            row=0;
            column=0;
            return;
        }
        row+=ISO_CHAR_HEIGHT;
        column=0;
        return;
    }
    if (ch=='\b') {
        if (column>0) {
            painted_chars_column--;
            painted_chars[painted_chars_row][painted_chars_column] = '\0';
            column-=ISO_CHAR_WIDTH;
            paint_char(column, row, '\0');
            return;
        }
    }
    painted_chars_column++;
    paint_char(column, row, ch);
    column+=ISO_CHAR_WIDTH;
    if (column>=(Platform_state.video.v_width)) {
        putc('\n');
    }
}
