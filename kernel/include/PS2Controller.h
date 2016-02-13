//
//  PS2Controller.h
//  OS
//
//  Created by Adam Kopeć on 2/12/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//


//  Totally unusable

#ifndef PS2Controller_h
#define PS2Controller_h

#include <sys/cdefs.h>

#define NULL_KEY    0x00
#define ESC_KEY     0x1B
#define LSHIFT_KEY  0x80
#define LCTRL_KEY   0x81
#define RCTRL_KEY   0x82
#define LALT_KEY    0x83
#define RALT_KEY    0x84
#define CAPS_KEY    0x85
#define NUM_KEY     0x86
#define SCROLL_KEY  0x87
#define PAUSE_KEY   0x88
#define F1_KEY      0x89
#define F2_KEY      0x8A
#define F3_KEY      0x8B
#define F4_KEY      0x8C
#define F5_KEY      0x8D
#define F6_KEY      0x8E
#define F7_KEY      0x8F
#define F8_KEY      0x90
#define F9_KEY      0x91
#define F10_KEY     0x92
#define F11_KEY     0x93
#define F12_KEY     0x94
#define RSHIFT_KEY  0x95

char keymap[][/* Special Keys, For now only: */ 0x03 ] = {
    
                /* Key */         /* Normal Key */        /* Shifted Key */       /* Escaped Key */
    /* 0x00 */  { /* NULL */        NULL_KEY,               NULL_KEY,               NULL_KEY        },
    /* 0x01 */  { /* Esc */         ESC_KEY,                ESC_KEY,                NULL_KEY        },
    /* 0x02 */  { /* 1 */           '1',                    '!',                    NULL_KEY        },
    /* 0x03 */  { /* 2 */           '2',                    '@',                    NULL_KEY        },
    /* 0x04 */  { /* 3 */           '3',                    '#',                    NULL_KEY        },
    /* 0x05 */  { /* 4 */           '4',                    '$',                    NULL_KEY        },
    /* 0x06 */  { /* 5 */           '5',                    '%',                    NULL_KEY        },
    /* 0x07 */  { /* 6 */           '6',                    '^',                    NULL_KEY        },
    /* 0x08 */  { /* 7 */           '7',                    '&',                    NULL_KEY        },  /* Don't forget to disable when using polling */
    /* 0x09 */  { /* 8 */           '8',                    '*',                    NULL_KEY        },
    /* 0x0A */  { /* 9 */           '9',                    '(',                    NULL_KEY        },
    /* 0x0B */  { /* 0 */           '0',                    ')',                    NULL_KEY        },
    /* 0x0C */  { /* - */           '-',                    '_',                    NULL_KEY        },
    /* 0x0D */  { /* = */           '=',                    '+',                    NULL_KEY        },
    /* 0x0E */  { /* Backspace */   '\b',                   '\b',                   NULL_KEY        },
    /* 0x0F */  { /* Tab */         '\t',                   NULL_KEY,               NULL_KEY        },
    /* 0x10 */  { /* Q */           'q',                    'Q',                    NULL_KEY        },
    /* 0x11 */  { /* W */           'w',                    'W',                    NULL_KEY        },
    /* 0x12 */  { /* E */           'e',                    'E',                    NULL_KEY        },
    /* 0x13 */  { /* R */           'r',                    'R',                    NULL_KEY        },
    /* 0x14 */  { /* T */           't',                    'T',                    NULL_KEY        },
    /* 0x15 */  { /* Y */           'y',                    'Y',                    NULL_KEY        },
    /* 0x16 */  { /* U */           'u',                    'U',                    NULL_KEY        },
    /* 0x17 */  { /* I */           'i',                    'I',                    NULL_KEY        },
    /* 0x18 */  { /* O */           'o',                    'O',                    NULL_KEY        },
    /* 0x19 */  { /* P */           'p',                    'P',                    NULL_KEY        },
    /* 0x1A */  { /* [ */           '[',                    '{',                    NULL_KEY        },
    /* 0x1B */  { /* ] */           ']',                    '}',                    NULL_KEY        },
    /* 0x1C */  { /* Enter */       '\n',                   '\n',                   '\n'            },
    /* 0x1D */  { /* Left Ctrl */   LCTRL_KEY,              LCTRL_KEY,              RCTRL_KEY       },
    /* 0x1E */  { /* A */           'a',                    'A',                    NULL_KEY        },
    /* 0x1F */  { /* S */           's',                    'S',                    NULL_KEY        },
    /* 0x20 */  { /* D */           'd',                    'D',                    NULL_KEY        },
    /* 0x21 */  { /* F */           'f',                    'F',                    NULL_KEY        },
    /* 0x22 */  { /* G */           'g',                    'G',                    NULL_KEY        },
    /* 0x23 */  { /* H */           'h',                    'H',                    NULL_KEY        },
    /* 0x24 */  { /* J */           'j',                    'J',                    NULL_KEY        },
    /* 0x25 */  { /* K */           'k',                    'K',                    NULL_KEY        },
    /* 0x26 */  { /* L */           'l',                    'L',                    NULL_KEY        },
    /* 0x27 */  { /* ; */           ';',                    ':',                    NULL_KEY        },
    /* 0x28 */  { /* ' */           '\'',                   '"',                    NULL_KEY        },
    /* 0x29 */  { /* ` */           '`',                    '~',                    NULL_KEY        },
    /* 0x2A */  { /* Left Shift */  LSHIFT_KEY,             LSHIFT_KEY,             NULL_KEY        },
    /* 0x2B */  { /* \ */           '\\',                   '|',                    NULL_KEY        },
    /* 0x2C */  { /* Z */           'z',                    'Z',                    NULL_KEY        },
    /* 0x2D */  { /* X */           'x',                    'X',                    NULL_KEY        },
    /* 0x2E */  { /* C */           'c',                    'C',                    NULL_KEY        },
    /* 0x2F */  { /* V */           'v',                    'V',                    NULL_KEY        },
    /* 0x30 */  { /* B */           'b',                    'B',                    NULL_KEY        },
    /* 0x31 */  { /* N */           'n',                    'N',                    NULL_KEY        },
    /* 0x32 */  { /* M */           'm',                    'M',                    NULL_KEY        },
    /* 0x33 */  { /* , */           ',',                    '<',                    NULL_KEY        },
    /* 0x34 */  { /* . */           '.',                    '>',                    NULL_KEY        },
    /* 0x35 */  { /* / */           '/',                    '?',                    '/'             },
    /* 0x36 */  { /* Right Shift */ RSHIFT_KEY,             RSHIFT_KEY,             NULL_KEY        },
    /* 0x37 */  { /* Keypad * */    '*',                    '*',                    NULL_KEY        },
    /* 0x38 */  { /* Left Alt */    LALT_KEY,               LALT_KEY,               RALT_KEY        },
    /* 0x39 */  { /* Space */       ' ',                    ' ',                    ' '             },
    /* 0x3A */  { /* CapsLock */    CAPS_KEY,               CAPS_KEY,               NULL_KEY        },
    /* 0x3B */  { /* F1 */          F1_KEY,                 F1_KEY,                 NULL_KEY        },
    /* 0x3C */  { /* F2 */          F2_KEY,                 F2_KEY,                 NULL_KEY        },
    /* 0x3D */  { /* F3 */          F3_KEY,                 F3_KEY,                 NULL_KEY        },
    /* 0x3E */  { /* F4 */          F4_KEY,                 F4_KEY,                 NULL_KEY        },
    /* 0x3F */  { /* F5 */          F5_KEY,                 F5_KEY,                 NULL_KEY        },
    /* 0x40 */  { /* F6 */          F6_KEY,                 F6_KEY,                 NULL_KEY        },
    /* 0x41 */  { /* F7 */          F7_KEY,                 F7_KEY,                 NULL_KEY        },
    /* 0x42 */  { /* F8 */          F8_KEY,                 F8_KEY,                 NULL_KEY        },
    /* 0x43 */  { /* F9 */          F9_KEY,                 F9_KEY,                 NULL_KEY        },
    /* 0x44 */  { /* F10 */         F10_KEY,                F10_KEY,                NULL_KEY        },
    /* 0x45 */  { /* NumLock */     NUM_KEY,                NUM_KEY,                NULL_KEY        },
    /* 0x46 */  { /* ScrollLock */  SCROLL_KEY,             SCROLL_KEY,             SCROLL_KEY      },
    /* 0x47 */  { /* Keypad 7 */    '7',                    '7',                    NULL_KEY        },
    /* 0x48 */  { /* Keypad 8 */    '8',                    '8',                    NULL_KEY        },  /* Arrow Up? */
    /* 0x49 */  { /* Keypad 9 */    '9',                    '9',                    NULL_KEY        },
    /* 0x4A */  { /* Keypad - */    '-',                    '-',                    NULL_KEY        },
    /* 0x4B */  { /* Keypad 4 */    '4',                    '4',                    NULL_KEY        },  /* Arrow Left? */
    /* 0x4C */  { /* Keypad 5 */    '5',                    '5',                    NULL_KEY        },
    /* 0x4D */  { /* Keypad 6 */    '6',                    '6',                    NULL_KEY        },  /* Arrow Right? */
    /* 0x4E */  { /* Keypad + */    '+',                    '+',                    NULL_KEY        },
    /* 0x4F */  { /* Keypad 1 */    '1',                    '1',                    NULL_KEY        },
    /* 0x50 */  { /* Keypad 2 */    '2',                    '2',                    NULL_KEY        },  /* Arrow Down? */
    /* 0x51 */  { /* Keypad 3 */    '3',                    '3',                    NULL_KEY        },
    /* 0x52 */  { /* Keypad 0 */    '0',                    '0',                    NULL_KEY        },
    /* 0x53 */  { /* Keypad . */    '.',                    '.',                    NULL_KEY        },
};

#endif /* PS2Controller_h */
