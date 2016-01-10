//
//  boot.h
//  OS
//
//  Created by Adam Kopeć on 1/10/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef boot_h
#define boot_h

#define BOOT_NULL_SEG       0x00
#define BOOT_CODE_SEG       0x08
#define BOOT_DATA_SEG       0x10
#define BOOT_CODE16_SEG     0x18
#define BOOT_DATA16_SEG     0x20
#define BOOT_LDT_SEG        0x28

#define BOOT_LDT_ENTRIES    1024
#define BOOT_LDT_SIZE       (BOOT_LDT_ENTRIES * 8)

/* Unused real-mode-accessable scratch memory. */
#define BOOT_REALMODE_SCRATCH   0x7C00

/*
 * The bootloader defines an LDT table which can be modified
 * by C code, for loading segments dynamically.
 */
#ifndef ASM
extern unsigned char LDT[BOOT_LDT_SIZE];
#endif

#endif /* boot_h */
