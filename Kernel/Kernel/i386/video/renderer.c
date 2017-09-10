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
//#include "../misc_protos.h"

#define MAX_NUMBER_OF_SCREENS 25

static uint32_t color_foreground = 0x00FFFFFF;
static uint32_t color_background = 0x00000000;

uint64_t Screen;
bool     use_screen_caching = false;
const bool     experimental = false;
/*char painted_chars[1000][1000];
unsigned long painted_chars_row    = 0;
unsigned long painted_chars_column = 0;*/

void vsputc(int ch);
void refresh_screen(void);

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
    uint32_t *where_screen;
    int       i;
    
    render_char(ch, rendered_char, 32);
    theChar = (uint32_t*)(rendered_char);
    
    where = (uint32_t*)(Platform_state.video.v_baseAddr + (y * Platform_state.video.v_rowBytes) + (x * 4));
    where_screen = (uint32_t*)(Screen + (y * Platform_state.video.v_rowBytes) + (x * 4));
    
    for (i = 0; i < ISO_CHAR_HEIGHT; i++) {
        uint32_t *store  = where;
        uint32_t *store2 = where_screen;
        int j;
        for (j = 0; j < 8; j++) {
            uint32_t val = *theChar++;
            val = (color_background & ~val) | (color_foreground & val);
            if (!use_screen_caching || !experimental) {
                *store++  = val;
            }
            if (use_screen_caching) {
                *store2++ = val;
            }
        }
        where = (uint32_t *)(((unsigned char*)where)+Platform_state.video.v_rowBytes);
        where_screen = (uint32_t *)(((unsigned char*) where_screen) + Platform_state.video.v_rowBytes);
    }
}

extern void bcopy(void *, void *, size_t);

static void
clear_line(unsigned int xx, unsigned int yy) {
    unsigned int start, end, i;
    start = xx;
    end = (unsigned int)((Platform_state.video.v_width) - 1);
    
    for (i = start; i <= end; i+=ISO_CHAR_WIDTH) {
        paint_char(i, yy, ' ');
    }
}

static void
clear_screen_(unsigned int xx, unsigned int yy, unsigned int bottom) {
    uint32_t *p, *endp, *_row;
    int linelongs, _col;
    int rowline, rowlongs;
    
    linelongs   = (int)(Platform_state.video.v_rowBytes * (ISO_CHAR_HEIGHT >> 2));
    rowline     = (int)((((Platform_state.video.v_depth + 7) / 8) * Platform_state.video.v_width) >> 2);
    rowlongs    = (int)(Platform_state.video.v_rowBytes >> 2);
    
    p    = (uint32_t *) Platform_state.video.v_baseAddr;
    endp = (uint32_t *) Platform_state.video.v_baseAddr;
    
    clear_line(xx, yy);
    if (yy < bottom - 1) {
        p    += (yy + 1) * linelongs;
        endp += bottom * linelongs;
    }
    
    for (_row = p; _row < endp; _row += rowlongs) {
        for (_col = 0; _col < rowline; _col++) {
            *(_row+_col) = 0;
        }
    }
}
void
scroll_up() {  // Fix half line after to scrolls
    uint32_t *from, *to, *to2, linelongs, i, line, rowline, rowscanline;
    linelongs   = (uint32_t)(Platform_state.video.v_rowBytes * (ISO_CHAR_HEIGHT >> 2));
    rowline     = (uint32_t)(Platform_state.video.v_rowBytes >> 2);
    rowscanline = (uint32_t)((((Platform_state.video.v_depth + 7) / 8) * Platform_state.video.v_width) >> 2);
    to   = (uint32_t *) Platform_state.video.v_baseAddr;
    if (use_screen_caching) {
        to2  = (uint32_t *) Screen;
        from = ((uint32_t *)Screen) + linelongs;
    } else {
        to2  = to;
        from = to + linelongs;
    }
    i    = (uint32_t)((Platform_state.video.v_height / ISO_CHAR_HEIGHT) - 1);
    
    while (i-- > 0) {
        for (line = 0; line < ISO_CHAR_HEIGHT; line++) {
            if (!use_screen_caching || !experimental) {
                bcopy(from, to,  (size_t)(((char *)(from+rowscanline) - (char *)from) << 2));
            }
            bcopy(from, to2, (size_t)(((char *)(from+rowscanline) - (char *)from) << 2));
            from += rowline;
            if (!use_screen_caching || !experimental) {
                to   += rowline;
            }
            to2  += rowline;
        }
    }
    clear_screen_(0, row, (uint32_t)(Platform_state.video.v_height / ISO_CHAR_HEIGHT));
    clear_screen_(0, (uint32_t)((Platform_state.video.v_height) - ISO_CHAR_HEIGHT), (uint32_t)(Platform_state.video.v_height / ISO_CHAR_HEIGHT));
    if (experimental) {
        refresh_screen();
    }
}

void
refresh_screen() {
    bcopy((void *) Screen, (void *) Platform_state.video.v_baseAddr, Platform_state.video.v_length);
}

void
vsputc(int ch) {
    if (ch=='\n'||ch=='\r') {
        if (row>=(Platform_state.video.v_height-(2*ISO_CHAR_HEIGHT))) {
            scroll_up();
            column = 0;
            return;
        }
        row+=ISO_CHAR_HEIGHT;
        column=0;
        return;
    }
    if (ch=='\b') {
        if (column>0) {
            column-=ISO_CHAR_WIDTH;
            paint_char(column, row, '\0');
            return;
        }
    }
    paint_char(column, row, ch);
    column+=ISO_CHAR_WIDTH;
    if (column>=(Platform_state.video.v_width)) {
        vsputc('\n');
    }
    
}
