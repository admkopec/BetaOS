//
//  SATAController.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/7/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "SATAController.h"
#include <kernel/kalloc.h>

#include <stdio.h>

void probe_port(HBA_MEM *abar);

void testSATA(void) {
    /*FIS_REG_H2D fis;
    memset(&fis, 0, sizeof(FIS_REG_H2D));
    fis.fis_type = FIS_TYPE_REG_H2D;
    fis.command  = 0xEC;               // ATA_CMD_IDENTIFY
    fis.device   = 0;                  // Master device
    fis.c = 1;                         // Write command register*/
    
    HBA_MEM mem;
    memset(&mem, 0, sizeof(HBA_MEM));
    probe_port(&mem);
}


#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM     0x96690101	// Port multiplier
#define AHCI_DEV_SATA   0x5A7A
#define AHCI_DEV_SATAPI 0x5A7A314
#define AHCI_DEV_SEMB   0x5E4DB
#define AHCI_DEV_PM     0x504D
#define AHCI_DEV_NULL   0x0000

static int check_type(HBA_PORT *port);

void probe_port(HBA_MEM *abar) {
    // Search disk in impelemented ports
    uint32_t pi = abar->pi;
    int i = 0;
    while (i<32)
    {
        if (pi & 1)
        {
            int dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                printf("SATA drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                printf("SATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                printf("SEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                printf("PM drive found at port %d\n", i);
            }
            else
            {
                printf("No drive found at port %d\n", i);
            }
        }
        
        pi >>= 1;
        i ++;
    }
}

// Check device type
static int check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;
    
    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;
    
    //if (det != HBA_PORT_DET_PRESENT)	// Check drive status
    //    return AHCI_DEV_NULL;
    //if (ipm != HBA_PORT_IPM_ACTIVE)
    //    return AHCI_DEV_NULL;
    
    switch (port->sig)
    {
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
