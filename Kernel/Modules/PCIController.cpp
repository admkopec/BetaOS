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
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif

extern vm_offset_t io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags);

uint32_t pciGetConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    
    address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    
    outl (0xCF8, address);
    return inl(0xCFC);
}

void pciWriteConfig(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data) {
    uint32_t address = (uint32_t)((((uint32_t)(bus)) << 16) | (((uint32_t)(slot)) << 11) | (((uint32_t)func) << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    outl(0xCFC, data);
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
    VendorID_ = (uint16_t)pciGetConfig(Bus, Slot, 0, 0);
}

void PCI::getDevice() {
    DeviceID_ = (uint16_t)(pciGetConfig(Bus, Slot, 0, 0) >> 16);
}

void PCI::getHeader() {
    HeaderType_ = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x0C) >> 16);
    ClassCode_  = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >> 24);
    SubClass_   = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >> 16);
    ProgIF_     = (uint8_t)(pciGetConfig(Bus, Slot, Function, 0x08) >>  8);
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
    
    ReadBar(index, &addressLow, &maskLow, Bus, Slot, Function);
    
    if (addressLow & 0x04) {            // 64-bit BAR
        uint32_t addressHigh;
        uint32_t maskHigh;
        
        ReadBar(index, &addressHigh, &maskHigh, Bus, Slot, Function);
        
        BAR_.u.address  = (void *)(((uintptr_t) addressHigh << 32) | (addressLow & ~0xF));
        BAR_.size       = ~(((uint64_t)maskHigh << 32) | (maskLow & ~0xF)) + 1;
        BAR_.flags      = addressLow & 0xF;
        
        //BAR_.u.address  = (void *)(io_map((vm_offset_t)BAR_.u.address, BAR_.size, VM_WIMG_IO));
        
        return 0x04;
    } else if (addressLow & 0x01) {     // IO BAR
        BAR_.u.port     = (uint16_t)(addressLow & ~0x3);
        BAR_.size       = (uint16_t)(~(maskLow & ~0x3) + 1);
        BAR_.flags      = addressLow & 0x3;
        
        return 0x01;
    } else {                            // 32-bit BAR
        BAR_.u.address  = (void *)((uintptr_t)(addressLow & ~0xF));
        BAR_.size       = ~(maskLow & ~0xF) + 1;
        BAR_.flags      = addressLow & 0xF;
        
        //BAR_.u.address  = (void *)(io_map((vm_offset_t)BAR_.u.address, BAR_.size, VM_WIMG_IO));
        
        return 0x00;
    }
}

uint16_t PCI::VendorID()  { return VendorID_;  }
uint16_t PCI::DeviceID()  { return DeviceID_;  }
uint8_t  PCI::ClassCode() { return ClassCode_; }
uint8_t  PCI::SubClass()  { return SubClass_;  }
uint8_t  PCI::ProgIF()    { return ProgIF_;    }
BAR      PCI::BAR()       { return BAR_;       }
