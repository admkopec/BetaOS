//
//  PCIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 1/11/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "PCIController.hpp"
#include <i386/pio.h>
#include <i386/machine_routines.h>

#ifdef DEBUG
#define DBG(x...) printf("PCIController: " x)
#else
#define DBG(x...)
#endif

extern "C" vm_offset_t io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags);

uint32_t pciGetConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    
    address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    
    outl (0xCF8, address);
    return inl(0xCFC >> (8 * (offset % 0x04)));
}

void pciWriteConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data) {
    uint32_t address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    outl(0xCFC, data);
}

void pciWriteConfig16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data) {
    uint32_t address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    outw(0xCFC + (offset & 0x02), data);
}

void pciWriteConfig8(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data) {
    uint32_t address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    outb(0xCFC + (offset & 0x03), data);
}

void PCI::init(int bus, int slot, int function) {
    Bus = bus; Slot = slot; Function = function;
    getVendor();
    if (VendorID_ == 0x0000 || VendorID_ == 0xFFFF) {
        Valid = false;
        return;
    }
    getDevice();
    getHeader();
}

void PCI::getVendor() {
    VendorID_ = (uint16_t)pciGetConfig(Bus, Slot, Function, 0);
}

void PCI::getDevice() {
    DeviceID_ = (uint16_t)(pciGetConfig(Bus, Slot, Function, 0) >> 16);
}

void PCI::getHeader() {
    HeaderType_ = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x0C) >> 16);
    ClassCode_  = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >> 24);
    SubClass_   = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >> 16);
    ProgIF_     = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >>  8);
    IntLine_    = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x3C) >>  0);
}

static void ReadBar(uint16_t index, uint32_t *address, uint32_t *mask, uint8_t bus, uint8_t slot, uint8_t func) {
    uint8_t offset = 0x10 + index * sizeof(uint32_t);
    *address = pciGetConfig(bus, slot, func, offset);
    pciWriteConfig(bus, slot, func, offset, 0xffffffff);
    *mask    = pciGetConfig(bus, slot, func, offset);
    pciWriteConfig(bus, slot, func, offset, *address);
}

int PCI::getBAR(uint16_t index) {
    uint32_t addressLow;
    uint32_t maskLow;
    
    ReadBar(index, (&addressLow), (&maskLow), Bus, Slot, Function);
    
    if (addressLow & 0x04) {            // 64-bit BAR
        uint32_t addressHigh;
        uint32_t maskHigh;
        
        ReadBar(index+1, (&addressHigh), (&maskHigh), Bus, Slot, Function);
        
        //BAR_.u.address  = (uint64_t)(((uintptr_t) addressHigh << 32) | (addressLow & ~0xF));
        BAR_.u.address  = (uint64_t)(((uint64_t)(addressLow & 0xFFFFFFF0) + ((uint64_t)(addressHigh & 0xFFFFFFFF) << 32)));
        BAR_.size       = ~(((uint64_t)maskHigh << 32) | (maskLow & ~0xF)) + 1;
        BAR_.flags      = addressLow & 0xF;
        
        BAR_.u.address  = (uint64_t)(io_map((vm_map_offset_t)(BAR_.u.address/* & ~3*/), round_page(BAR_.size), VM_WIMG_IO));
        
        return 0x04;
    } else if (addressLow & 0x01) {     // IO BAR
        BAR_.u.port     = (uint16_t)(addressLow & 0xFFFFFFFC/*~0x3*/);
        BAR_.size       = (uint16_t)(~(maskLow & ~0x3) + 1);
        BAR_.flags      = addressLow & 0x3;
        
        return 0x01;
    } else {                            // 32-bit BAR
        BAR_.u.address  = (uint64_t)((addressLow & 0xFFFFFFF0/*~0xF*/));
        BAR_.size       = ~(maskLow & ~0xF) + 1;
        BAR_.flags      = addressLow & 0xF;
        
        BAR_.u.address  = (uint64_t)(io_map((vm_map_offset_t)(BAR_.u.address/* & ~3*/), round_page(BAR_.size), VM_WIMG_IO));
        
        return 0x00;
    }
}

uint32_t PCI::Read32(uint8_t offset) {
    return pciGetConfig(Bus, Slot, Function, offset);
}

void PCI::Write32(uint8_t offset, uint32_t data) {
    pciWriteConfig(Bus, Slot, Function, offset, data);
}

void PCI::Write16(uint8_t offset, uint16_t data) {
    pciWriteConfig16(Bus, Slot, Function, offset, data);
}

void PCI::Write8(uint8_t offset, uint8_t data) {
    pciWriteConfig8(Bus, Slot, Function, offset, data);
}

void PCI::EnableBusMastering() {
    uint32_t Command = Read32(0x04);
    Command |= 0x04;
    Write32(0x04, Command);
}

uint16_t PCI::VendorID()    { return VendorID_;  }
uint16_t PCI::DeviceID()    { return DeviceID_;  }
uint8_t  PCI::ClassCode()   { return ClassCode_; }
uint8_t  PCI::SubClass()    { return SubClass_;  }
uint8_t  PCI::ProgIF()      { return ProgIF_;    }
uint8_t  PCI::IntLine()     { return IntLine_;   }
BAR      PCI::BAR()         { return BAR_;       }
