//
//  SATAController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 2/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SATAController.hpp"

#define super Controller
#define Log(x ...) printf("SATAController: " x)

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

// Start command engine
void
SATA::start_cmd(HBA_PORT *port) {
    // Wait until CR (bit15) is cleared
    while (port->cmd & (1 << 15));
    
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= (1 << 4);
    port->cmd |= (1 << 0);
}

// Stop command engine
void
SATA::stop_cmd(HBA_PORT *port) {
    // Clear ST (bit0)
    port->cmd &= ~(1 << 0);
    
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
    {
        if (port->cmd & (1 << 14))
            continue;
        if (port->cmd & (1 << 15))
            continue;
        break;
    }
    
    // Clear FRE (bit4)
    port->cmd &= ~(1 << 4);
}

// Find a free command list slot
int
SATA::find_cmdslot(HBA_PORT *port) {
    // If not set in SACT and CI, the slot is free
    uint16_t slots = (port->sact | port->ci);
    for (int i=0; i</*cmdslots*/25; i++) {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    Log("Cannot find free command list entry\n");
    return -1;
}

void
SATA::port_rebase(HBA_PORT *port, int portno) {
    stop_cmd(port);	// Stop command engine
    
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = 0x400000 + (portno<<10);
    port->clbu = 0;
    memset((void*)(port->clb), 0, 1024);
    
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = 0x400000 + (32<<10) + (portno<<8);
    port->fbu = 0;
    memset((void*)(port->fb), 0, 256);
    
    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
    for (int i=0; i<32; i++) {
        cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = 0x400000 + (40<<10) + (portno<<13) + (i<<8);
        cmdheader[i].ctbau = 0;
        memset((void*)(cmdheader[i].ctba), 0, 256);
    }
    
    start_cmd(port);	// Start command engine
}

int
SATA::init(PCI *header) {
    if (!(header->ClassCode() == 0x01 && header->SubClass() == 0x06)) {
        return -1;
    }
    Log("SATA found!\n");
    Log("Vendor ID: %X, Device ID: %X\n", header->VendorID(), header->DeviceID());
    if (header->ProgIF() == 0x01) {
        Log("AHCI found!\n");
    } else {
        Log("Unknown Prog IF: %X\n", header->ProgIF());
        return -2;
    }
    
    header->getBAR(5);
    
    address = ((HBA_MEM*)header->BAR().u.address);
    
    uint32_t pi = address->pi;
    int i = 0;
    while (i<32) {
        if (pi & 1) {
            int dt = check_type(&address->ports[i]);
            if (dt == AHCI_DEV_SATA) {
                Log("SATA drive found at port %d\n", i);
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
        i ++;
    }

    Used_ = true;
    
    return 0;
}

void
SATA::start() {
    super::start();
}

void
SATA::stop() {
    super::stop();
}
