//
//  SATAController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 2/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef SATAController_hpp
#define SATAController_hpp

#include <stdio.h>
#include <stdint.h>
#include "Controller.hpp"

#define	SATA_SIG_ATA            0x00000101	// SATA drive
#define	SATA_SIG_ATAPI          0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB           0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM             0x96690101	// Port multiplier
#define HBA_PORT_IPM_ACTIVE     0x1
#define HBA_PORT_DET_PRESENT    0x3
#define HBA_NO_DEVICE           0x0
#define AHCI_DEV_SATA           0x5A7A
#define AHCI_DEV_SATAPI         0x5A7A314
#define AHCI_DEV_SEMB           0x5E4DB
#define AHCI_DEV_PM             0x504D
#define AHCI_DEV_NULL           0x0000

typedef volatile struct tagHBA_PORT {
    uint32_t	clb;		// 0x00, command list base address, 1K-uint8_t aligned
    uint32_t	clbu;		// 0x04, command list base address upper 32 bits
    uint32_t	fb;         // 0x08, FIS base address, 256-uint8_t aligned
    uint32_t	fbu;		// 0x0C, FIS base address upper 32 bits
    uint32_t	is;         // 0x10, interrupt status
    uint32_t	ie;         // 0x14, interrupt enable
    uint32_t	cmd;		// 0x18, command and status
    uint32_t	rsv0;		// 0x1C, Reserved
    uint32_t	tfd;		// 0x20, task file data
    uint32_t	sig;		// 0x24, signature
    uint32_t	ssts;		// 0x28, SATA status (SCR0:SStatus)
    uint32_t	sctl;		// 0x2C, SATA control (SCR2:SControl)
    uint32_t	serr;		// 0x30, SATA error (SCR1:SError)
    uint32_t	sact;		// 0x34, SATA active (SCR3:SActive)
    uint32_t	ci;         // 0x38, command issue
    uint32_t	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
    uint32_t	fbs;		// 0x40, FIS-based switch control
    uint32_t	rsv1[11];	// 0x44 ~ 0x6F, Reserved
    uint32_t	vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM {
    // 0x00 - 0x2B, Generic Host Control
    uint32_t	cap;		// 0x00, Host capability
    uint32_t	ghc;		// 0x04, Global host control
    uint32_t	is;         // 0x08, Interrupt status
    uint32_t	pi;         // 0x0C, Port implemented
    uint32_t	vs;         // 0x10, Version
    uint32_t	ccc_ctl;	// 0x14, Command completion coalescing control
    uint32_t	ccc_pts;	// 0x18, Command completion coalescing ports
    uint32_t	em_loc;		// 0x1C, Enclosure management location
    uint32_t	em_ctl;		// 0x20, Enclosure management control
    uint32_t	cap2;		// 0x24, Host capabilities extended
    uint32_t	bohc;		// 0x28, BIOS/OS handoff control and status
    
    // 0x2C - 0x9F, Reserved
    uint8_t	rsv[0xA0-0x2C];
    
    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t	vendor[0x100-0xA0];
    
    // 0x100 - 0x10FF, Port control registers
    HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;

typedef struct tagHBA_CMD_HEADER {
    // DW0
    uint8_t    cfl:5;      // Command FIS length in uint32_tS, 2 ~ 16
    uint8_t    a:1;        // ATAPI
    uint8_t    w:1;        // Write, 1: H2D, 0: D2H
    uint8_t    p:1;        // Prefetchable
    
    uint8_t    r:1;        // Reset
    uint8_t    b:1;        // BIST
    uint8_t    c:1;        // Clear busy upon R_OK
    uint8_t    rsv0:1;     // Reserved
    uint8_t    pmp:4;      // Port multiplier port
    
    uint16_t    prdtl;      // Physical region descriptor table length in entries
    
    // DW1
    volatile
    uint32_t   prdbc;      // Physical region descriptor uint8_t count transferred
    
    // DW2, 3
    uint32_t   ctba;       // Command table descriptor base address
    uint32_t   ctbau;      // Command table descriptor base address upper 32 bits
    
    // DW4 - 7
    uint32_t   rsv1[4];    // Reserved
} HBA_CMD_HEADER;

class SATA : public Controller {
    HBA_MEM* address;
    static int      check_type(HBA_PORT* port);
    static void     start_cmd(HBA_PORT* port);
    static void     stop_cmd(HBA_PORT* port);
    static void     port_rebase(HBA_PORT* port, int portno);
    
    int find_cmdslot(HBA_PORT *port);
    
public:
    virtual int  init(PCI *header) override;
    virtual void start() override;
    virtual void stop() override;
};

#endif /* SATAController_hpp */
