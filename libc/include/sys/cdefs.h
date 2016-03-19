//
//  cdefs.h
//  BetaOS
//
//  Created by Adam Kopeć on 12/11/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#include <version.h>

#include <stdint.h>

#define __myos_libc 1

#ifndef _GCC_WRAP_STDINT_H
/* If stdint.h doesn't load */

typedef long long int64;
typedef long long s64_t;
typedef unsigned long long uint64;
typedef unsigned long long u64_t;

typedef long int32;
typedef long s32_t;
typedef unsigned long uint32;
typedef unsigned long u32_t;

typedef int int16;
typedef int s16_t;
typedef unsigned int uint16;
typedef unsigned int u16_t;

typedef char int8;
typedef char s8_t;
typedef unsigned char uint8;
typedef unsigned char u8_t;

#endif

/* Useful macros */

#define NULL   ((void*)0)

#define offsetof(type, member)  ((uint32)(&((type*)NULL)->member))
#define arraysize(var)          (sizeof(var) / sizeof((var)[0]))
#define roundup(x, y)           (((x) + ((y) - 1)) / (y))

#define PACKED       __attribute__ ((__packed__))
#define ALIGNED(n)   __attribute__ ((aligned(n)))
#define fastcall     __attribute__ ((fastcall))

#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define MAX(a, b)   ((a) > (b) ? (a) : (b))


/* PIC */

#define PIC1            0x20		/* IO base address for master PIC */
#define PIC2            0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA       (PIC2+1)

#define PIC_EOI         0x20		/* End-of-interrupt command code */

#define ICW1_ICW4       0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE     0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08		/* Level triggered (edge) mode */
#define ICW1_INIT       0x10		/* Initialization - required! */

#define ICW4_8086       0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM       0x10		/* Special fully nested (not) */

#define PIC1_CMD        0x20
//#define PIC1_DATA       0x21
#define PIC2_CMD        0xA0
//#define PIC2_DATA       0xA1
#define PIC_READ_IRR    0x0a        /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    0x0b        /* OCW3 irq service next CMD read */

#endif
