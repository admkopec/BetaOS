//
//  SATAController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 2/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SATAController.hpp"
#include <i386/machine_routines.h>
#include <stdlib.h>

#define super Controller
#define Log(x ...) printf("SATAController: " x)
#ifdef DEBUG
#define DBG(x ...) printf("SATAController: " x)
#else
#define DBG(x ...)
#endif

extern "C" { vm_offset_t io_map(vm_offset_t phys_addr, vm_size_t size, unsigned int flags); }

uintptr_t BASE;

int
SATA::check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;
    
    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;
    
    if (det != HBA_PORT_DET_PRESENT)	// Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;
    
    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}

OSReturn
SATA::init(PCI *header) {
    if (!(header->ClassCode() == 0x01 && header->SubClass() == 0x06)) {
        return kOSReturnFailed;
    }
    Log("SATA found!\n");
    Log("Vendor ID: %x, Device ID: %x\n", header->VendorID(), header->DeviceID());
    if (header->ProgIF() == 0x01) {
        Log("AHCI found!\n");
    } else {
        Log("Unknown Prog IF: %x\n", header->ProgIF());
        return kOSReturnError;
    }
    
    header->getBAR(5);
    
    address = ((HBA_MEM*)header->BAR().u.address);
    BASE    = (uintptr_t)malloc(4096*100);
    
    BASE -= KERNEL_BASE;
    
    int LastDevice = 0;
    
    uint32_t pi = address->pi;
    int i = 0;
    while (i<32) {
        if (pi & 1) {
            int dt = check_type(&address->ports[i]);
            if (dt == AHCI_DEV_SATA) {
                Log("SATA drive found at port %d\n", i);
                SATADevice* sataDevice = new SATADevice();
                if (sataDevice->init(&address->ports[i], i) == kOSReturnSuccess) {
                    DBG("Succesfully set up Device!\n");
                    SATADevices[LastDevice] = sataDevice;
                    LastDevice++;
                }
            }
            else if (dt == AHCI_DEV_SATAPI) {
                Log("SATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB) {
                Log("SEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM) {
                Log("PM drive found at port %d\n", i);
            }
            else {
                //Log("No drive found at port %d\n", i);
            }
        }
        
        pi >>= 1;
        i++;
    }

    NameString = (char*)"SATAController (AHCI)";
    Used_ = true;
    
    return kOSReturnSuccess;
}

void
SATA::start() {
    super::start();
}

void
SATA::stop() {
    super::stop();
}


OSReturn
SATADevice::init(HBA_PORT *port, int PortNum) {
    Port        = port;
    PortNumber  = PortNum;
    rebase();
    
    // Set up
    uint16_t* buf = (uint16_t*)malloc(4096);
    port->is = 0xffff;
    int spin  = 0;
    int count = 1;
    int slot = find_cmdslot();
    if (slot == -1) {
        Log("Error! Couldn't find free slot!\n");
        return kOSReturnError;
    }
    cmdHeader += slot;
    cmdHeader->cfl   = sizeof(FIS_REG_H2D)/sizeof(uint32_t);
    cmdHeader->w     = 0;
    cmdHeader->c     = 1;
    cmdHeader->p     = 1;
    cmdHeader->prdtl = (uint16_t)((count-1)>>4) + 1;
    
    HBA_CMD_TBL *cmdTable = (HBA_CMD_TBL *)(((((uintptr_t)cmdHeader->ctbau << 32) + (uintptr_t)(cmdHeader->ctba))) + KERNEL_BASE);
    memset(cmdTable, 0, sizeof(HBA_CMD_TBL) + (cmdHeader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
    int i = 0;
    for (i = 0; i<cmdHeader->prdtl-1; i++) {
        cmdTable->prdt_entry[i].dba  = (uint32_t) (uintptr_t)(buf - KERNEL_BASE);
        cmdTable->prdt_entry[i].dbau = (uint32_t)((uintptr_t)(buf - KERNEL_BASE) >> 32);
        cmdTable->prdt_entry[i].dbc = 8*1024-1;
        cmdTable->prdt_entry[i].i = 1;
        buf += 4*1024;
        count -= 16;
    }
    cmdTable->prdt_entry[i].dba = (uint32_t) (uintptr_t)(buf - KERNEL_BASE);
    cmdTable->prdt_entry[i].dba = (uint32_t)((uintptr_t)(buf - KERNEL_BASE) >> 32);
    cmdTable->prdt_entry[i].dbc = (count<<9)-1;
    cmdTable->prdt_entry[i].i = 1;
    
    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdTable->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;    // Command
    cmdfis->command = 0x25;
    cmdfis->lba0 = (uint8_t)1;
    cmdfis->lba1 = (uint8_t)(1>>8);
    cmdfis->lba2 = (uint8_t)(1>>16);
    cmdfis->device = 1<<6;    // LBA mode
    
    cmdfis->lba3 = (uint8_t)(1>>24);
    cmdfis->lba4 = (uint8_t)1;
    cmdfis->lba5 = (uint8_t)(1>>8);
    
    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;
    
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((Port->tfd & (0x80 | 0x08)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        Log("Error! Port is hung\n");
        return kOSReturnError;
    }
    
    Port->ci = 1<<slot;
    
    while (1) {
        if ((Port->ci & (1<<slot)) == 0)
            break;
        if (Port->is & (1 << 30)) {
            Log("Read disk error\n");
            return kOSReturnFailed;
        }
    }
    
    if (Port->is & (1 << 30)) {
        Log("Read disk Error!\n");
        return kOSReturnFailed;
    }
    
    // The below loop waits until the port is no longer busy before issuing a new command
    spin = 0;
    while ((Port->tfd & (0x80 | 0x08)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        Log("Error! Port is hung\n");
        return kOSReturnError;
    }
    
    cmdfis->command = 0xE6;
    
    Port->ci = 1<<slot;
    
//  Log("Buf == %s\n", buf);
    return kOSReturnSuccess;
}

// Start command engine
void
SATADevice::start_cmd(void) {
    // Wait until CR (bit15) is cleared
    while (Port->cmd & (1 << 15));
    
    // Set FRE (bit4) and ST (bit0)
    Port->cmd |= (1 << 4);
    Port->cmd |= (1 << 0);
}

// Stop command engine
void
SATADevice::stop_cmd(void) {
    // Clear ST (bit0)
    Port->cmd &= ~(1 << 0);
    
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1) {
        if (Port->cmd & (1 << 14))
            continue;
        if (Port->cmd & (1 << 15))
            continue;
        break;
    }
    
    // Clear FRE (bit4)
    Port->cmd &= ~(1 << 4);
}

// Find a free command list slot
int
SATADevice::find_cmdslot(void) {
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (Port->sact | Port->ci);
    for (int i=0; i</*cmdslots*/25; i++) {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    Log("Cannot find free command list entry\n");
    return -1;
}

void
SATADevice::rebase() {
    stop_cmd();
    Port->clb  = (uint32_t) (BASE + (PortNumber<<10));
    Port->clbu = (uint32_t)((BASE + (PortNumber<<10)) >> 32);
    memset((void *)((BASE + KERNEL_BASE) + (PortNumber<<10)), 0, 1024);
    
    Port->fb  = (uint32_t) (BASE + (32<<10) + (PortNumber<<8));
    Port->fbu = (uint32_t)((BASE + (32<<10) + (PortNumber<<8)) >> 32);
    memset((void *)((BASE + KERNEL_BASE) + (32<<10) + (PortNumber<<8)), 0, 256);
    
    cmdHeader = (HBA_CMD_HEADER *)(((BASE + KERNEL_BASE) + (PortNumber<<10)));
    
    for (int j = 0; j<32; j++) {
        cmdHeader[j].prdtl = 8;
        cmdHeader[j].ctba  = (uint32_t) (BASE + (40<<10) + (PortNumber<<13) + (j<<8));
        cmdHeader[j].ctbau = (uint32_t)((BASE + (40<<10) + (PortNumber<<13) + (j<<8)) >> 32);
        memset((void*)((BASE + KERNEL_BASE) + (40<<10) + (PortNumber<<13) + (j<<8)), 0, 256);
    }
    
    start_cmd();
}
