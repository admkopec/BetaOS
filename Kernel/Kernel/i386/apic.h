//
//  apic.h
//  BetaOS
//
//  Created by Adam Kopeć on 6/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef apic_h
#define apic_h

#define IOAPIC_START			0xFEC00000
#define	IOAPIC_SIZE             0x00000020

#define IOAPIC_RSELECT			0x00000000
#define IOAPIC_RWINDOW			0x00000010
#define IOA_R_ID                0x00
#define	IOA_R_ID_SHIFT          24
#define IOA_R_VERSION			0x01
#define	IOA_R_VERSION_MASK      0xFF
#define	IOA_R_VERSION_ME_SHIFT	16
#define	IOA_R_VERSION_ME_MASK	0xFF
#define IOA_R_REDIRECTION		0x10
#define IOA_R_R_VECTOR_MASK     0x000FF
#define	IOA_R_R_DM_MASK         0x00700
#define	IOA_R_R_DM_FIXED        0x00000
#define	IOA_R_R_DM_LOWEST       0x00100
#define	IOA_R_R_DM_NMI          0x00400
#define	IOA_R_R_DM_RESET        0x00500
#define	IOA_R_R_DM_EXTINT       0x00700
#define	IOA_R_R_DEST_LOGICAL	0x00800
#define	IOA_R_R_DS_PENDING      0x01000
#define	IOA_R_R_IP_PLRITY_LOW	0x02000
#define	IOA_R_R_TM_LEVEL        0x08000
#define	IOA_R_R_MASKED          0x10000

#endif /* apic_h */
