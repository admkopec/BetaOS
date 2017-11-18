//
//  apic.c
//  Kernel
//
//  Created by Adam Kopeć on 5/8/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "apic.h"
#include "misc_protos.h"
#include "pmap.h"
#include "pio.h"
#include <stdio.h>

uint32_t    IOAPICAddressPhys;
vm_offset_t IOAPICAddressV;

extern bool VendorisApple;

void WriteReg(uint8_t offset, uint32_t value) {
    *(volatile uint32_t *)(IOAPICAddressV + IOAPIC_RSELECT) = offset;
    *(volatile uint32_t *)(IOAPICAddressV + IOAPIC_RWINDOW) = value;
}

uint32_t ReadReg(uint8_t offset) {
    *(volatile uint32_t *)(IOAPICAddressV + IOAPIC_RSELECT) = offset;
    return *(volatile uint32_t *)(IOAPICAddressV + IOAPIC_RWINDOW);
}

void APICInit(void) {
    IOAPICAddressV = io_map(IOAPICAddressPhys, IOAPIC_SIZE, VM_WIMG_IO);
    
    // Mask PIC IRQs
    outb(0x20, 0x10 | 0x01);
    io_wait();
    outb(0xA0, 0x10 | 0x01);
    io_wait();
    outb(0x21, 0xD0);
    io_wait();
    outb(0xA1, 0xD8);
    io_wait();
    outb(0x21, 4);
    io_wait();
    outb(0xA1, 2);
    io_wait();
    
    
    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();
    if (!VendorisApple) {
        outb(0x21, ((0xFF & ~(1 << 0)) & ~(1 << 1)));
    } else {
        outb(0x21, (0xFF & ~(1 << 0)));
    }
    outb(0xA1, 0xFF);
    
    uint32_t NumberOfIRQs = ((ReadReg(IOA_R_VERSION) >> IOA_R_VERSION_ME_SHIFT) & IOA_R_VERSION_ME_MASK) + 1;
    
    printf("Max IRQs: %d\n", NumberOfIRQs);
    
    for (uint16_t i = 0; i < NumberOfIRQs; ++i) {
        WriteReg(IOA_R_REDIRECTION + i * 2, (uint32_t)((uint64_t)(1 << 16)));
        WriteReg(IOA_R_REDIRECTION + i * 2 + 1, (uint32_t)(((uint64_t)(1 << 16)) >> 32));
    }
    
    /*WriteReg(IOA_R_REDIRECTION + 1 * 2, (uint32_t)((uint64_t)(0 << 16)));
    WriteReg(IOA_R_REDIRECTION + 1 * 2 + 1, (uint32_t)(((uint64_t)(0 << 16)) >> 32));*/
}
