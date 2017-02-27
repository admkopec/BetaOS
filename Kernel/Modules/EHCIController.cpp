//
//  EHCIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 1/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "EHCIController.hpp"

#define USB_FULL_SPEED  0x00
#define USB_LOW_SPEED   0x01
#define USB_HIGH_SPEED  0x02
#define USB_SUPER_SPEED 0x03

// TD Link Pointer
#define PTR_TERMINATE                   (1 << 0)

#define PTR_TYPE_MASK                   (3 << 1)
#define PTR_ITD                         (0 << 1)
#define PTR_QH                          (1 << 1)
#define PTR_SITD                        (2 << 1)
#define PTR_FSTN                        (3 << 1)

// Host Controller Structural Parameters Register

#define HCSPARAMS_N_PORTS_MASK          (15 <<  0)   // Number of Ports
#define HCSPARAMS_PPC                   (1  <<  4)   // Port Power Control
#define HCSPARAMS_PORT_ROUTE            (1  <<  7)   // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK            (15 <<  8)   // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT                   8
#define HCSPARAMS_N_CC_MASK             (15 << 12)   // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT                   12
#define HCSPARAMS_P_INDICATOR           (1  << 16)   // Port Indicator
#define HCSPARAMS_DPN_MASK              (15 << 20)   // Debug Port Number
#define HCSPARAMS_DPN_SHIFT                    20

// Host Controller Capability Parameters Register

#define HCCPARAMS_64_BIT                (1   << 0)  // 64-bit Addressing Capability
#define HCCPARAMS_PFLF                  (1   << 1)  // Programmable Frame List Flag
#define HCCPARAMS_ASPC                  (1   << 2)  // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK              (15  << 4)  // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK             (255 << 8)  // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT                    8

// USB Command Register

#define CMD_RS                          (1   <<  0)    // Run/Stop
#define CMD_HCRESET                     (1   <<  1)    // Host Controller Reset
#define CMD_FLS_MASK                    (3   <<  2)    // Frame List Size
#define CMD_FLS_SHIFT                            2
#define CMD_PSE                         (1   <<  4)    // Periodic Schedule Enable
#define CMD_ASE                         (1   <<  5)    // Asynchronous Schedule Enable
#define CMD_IOAAD                       (1   <<  6)    // Interrupt on Async Advance Doorbell
#define CMD_LHCR                        (1   <<  7)    // Light Host Controller Reset
#define CMD_ASPMC_MASK                  (3   <<  8)    // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT                          8
#define CMD_ASPME                       (1   << 11)    // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK                    (255 << 16)    // Interrupt Threshold Control
#define CMD_ITC_SHIFT                           16

// USB Status Register

#define STS_USBINT                      (1 << 0)    // USB Interrupt
#define STS_ERROR                       (1 << 1)    // USB Error Interrupt
#define STS_PCD                         (1 << 2)    // Port Change Detect
#define STS_FLR                         (1 << 3)    // Frame List Rollover
#define STS_HSE                         (1 << 4)    // Host System Error
#define STS_IOAA                        (1 << 5)    // Interrupt on Async Advance
#define STS_HCHALTED                    (1 << 12)   // Host Controller Halted
#define STS_RECLAMATION                 (1 << 13)   // Reclamation
#define STS_PSS                         (1 << 14)   // Periodic Schedule Status
#define STS_ASS                         (1 << 15)   // Asynchronous Schedule Status

// Port Status and Control Registers

#define PORT_CONNECTION                 (1  << 0)    // Current Connect Status
#define PORT_CONNECTION_CHANGE          (1  << 1)    // Connect Status Change
#define PORT_ENABLE                     (1  << 2)    // Port Enabled
#define PORT_ENABLE_CHANGE              (1  << 3)    // Port Enable Change
#define PORT_OVER_CURRENT               (1  << 4)    // Over-current Active
#define PORT_OVER_CURRENT_CHANGE        (1  << 5)    // Over-current Change
#define PORT_FPR                        (1  << 6)    // Force Port Resume
#define PORT_SUSPEND                    (1  << 7)    // Suspend
#define PORT_RESET                      (1  << 8)    // Port Reset
#define PORT_LS_MASK                    (3  << 10)   // Line Status
#define PORT_LS_SHIFT                          10
#define PORT_POWER                      (1  << 12)   // Port Power
#define PORT_OWNER                      (1  << 13)   // Port Owner
#define PORT_IC_MASK                    (3  << 14)   // Port Indicator Control
#define PORT_IC_SHIFT                          14
#define PORT_TC_MASK                    (15 << 16)   // Port Test Control
#define PORT_TC_SHIFT                          16
#define PORT_WKCNNT_E                   (1  << 20)   // Wake on Connect Enable
#define PORT_WKDSCNNT_E                 (1  << 21)   // Wake on Disconnect Enable
#define PORT_WKOC_E                     (1  << 22)   // Wake on Over-current Enable
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)


// Endpoint Characteristics
#define QH_CH_DEVADDR_MASK              0x0000007f  // Device Address
#define QH_CH_INACTIVE                  0x00000080  // Inactive on Next Transaction
#define QH_CH_ENDP_MASK                 0x00000f00  // Endpoint Number
#define QH_CH_ENDP_SHIFT                         8
#define QH_CH_EPS_MASK                  0x00003000  // Endpoint Speed
#define QH_CH_EPS_SHIFT                         12
#define QH_CH_DTC                       0x00004000  // Data Toggle Control
#define QH_CH_H                         0x00008000  // Head of Reclamation List Flag
#define QH_CH_MPL_MASK                  0x07ff0000  // Maximum Packet Length
#define QH_CH_MPL_SHIFT                         16
#define QH_CH_CONTROL                   0x08000000  // Control Endpoint Flag
#define QH_CH_NAK_RL_MASK               0xf0000000  // Nak Count Reload
#define QH_CH_NAK_RL_SHIFT                      28

// Endpoint Capabilities
#define QH_CAP_INT_SCHED_SHIFT                   0
#define QH_CAP_INT_SCHED_MASK           0x000000ff  // Interrupt Schedule Mask
#define QH_CAP_SPLIT_C_SHIFT                     8
#define QH_CAP_SPLIT_C_MASK             0x0000ff00  // Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT                   16
#define QH_CAP_HUB_ADDR_MASK            0x007f0000  // Hub Address
#define QH_CAP_PORT_MASK                0x3f800000  // Port Number
#define QH_CAP_PORT_SHIFT                       23
#define QH_CAP_MULT_MASK                0xc0000000  // High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT                       30

int
EHCI::init(PCI *pci) {
    if (!(pci->ClassCode() == PCI_USB_CLASS && pci->SubClass() == PCI_USB_SUBCLASS && pci->ProgIF() == PCI_USB_EHCI)) {
        return -1;
    }
    
    if (pci->BAR().size != 0) { // Fix condition
        printf("EHCIController: ERROR: This EHCIController has been already initialized with different EHCI PCI Device!\n");
        return -3;
    }
    
    pci->getBAR(0);
    if (pci->BAR().flags & 0x01) {
        printf("EHCIController: ERROR: Not using MMIO BAR!\n");
        return -2;
    }
    
    printf("EHCIController: EHCI found\n");
    printf("EHCIController: Vendor: %X Device: %X\n", pci->VendorID(), pci->DeviceID());
    
    return -2;
    
    Caps = (CapRegisters *)(uintptr_t)(pci->BAR().u.address);
    Ops  = (OpRegisters  *)(uintptr_t)((uintptr_t)(pci->BAR().u.address) + Caps->capLength);
    
    //return -2;
    
    // Nothing below is working
    
    memset(QHpool, 0, sizeof(EhciQH) * MAX_QH);
    memset(TDpool, 0, sizeof(EhciTD) * MAX_TD);
    
    EhciQH *QH = QHpool;
    EhciQH *end = QHpool + MAX_QH;
    for (QH = QHpool; QH != end; ++QH) {
        if (!QH->active) {
            QH->active = 1;
            break;
        }
    } if (QH == end) {
        printf("EHCIController: ERROR: Couldn't allocate QH!\n");
        return -2;
    }
    
    QH->QueueHeadHorizontalLinkPointer = (uint32_t)(uintptr_t)QH | PTR_QH;
    QH->EndpointCharacteristics        = QH_CH_H;
    QH->EndpointCapabilities           = 0;
    QH->curLink                        = 0;
    QH->nextLink                       = PTR_TERMINATE;
    QH->altLink                        = 0;
    QH->token                          = 0;
    
    for (uint16_t i = 0; i < 5; ++i) {
        QH->buffer[i]    = 0;
        QH->extBuffer[i] = 0;
    }
    
    QH->transfer = 0;
    QH->qhLink.prev = &QH->qhLink;
    QH->qhLink.next = &QH->qhLink;
    
    PeriodicQH = QH;
    
    for (uint16_t i = 0; i < 1024; ++i) {
        FrameList[i] = PTR_QH | (uint32_t)(uintptr_t)QH;
    }
    
    uint16_t EECP = (Caps->hccParams & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;
    if (EECP >= 0x40) {
        // Do something about Legacy Support
    }
    
    Ops->USBInterrupt     = 0;
    Ops->frameIndex       = 0;
    Ops->periodicListBase = (uint32_t)(uintptr_t)FrameList;
    Ops->asyncListAddr    = (uint32_t)(uintptr_t)AsyncQH;
    Ops->ctrlDsSegment    = 0;
    Ops->USBStatus        = 0x3F;
    
    Ops->USBCommad = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
    while (Ops->USBStatus & STS_HCHALTED) { }   // Poll For ACK
    Ops->configFlag = 1;
    for (int i = 0; i < 10; i++) { printf(""); }
    
    return 0;
}

void
EHCI::start() {
    printf("EHCIController: Starting\n");
    return;
    // Not working
    uint32_t portCount = Caps->hcsParams & HCSPARAMS_N_PORTS_MASK;
    for (uint32_t port = 0; port < portCount; port++) {
        uint32_t status = ResetPort(port);
        
        if (status & PORT_ENABLE) {
            uint32_t speed = USB_HIGH_SPEED;
            printf("Found Device at port %X with speed: %X\n", port, speed);
            // Set Up a Device here
        }
    }
}

void
EHCI::PortSet(volatile uint32_t *reg, uint32_t data) {
    uint32_t status  = *reg;
             status |= data;
             status &= ~PORT_RWC;
             *reg    = status;
}

void
EHCI::PortClear(volatile uint32_t *reg, uint32_t data) {
    uint32_t status  = *reg;
             status &= ~PORT_RWC;
             status &= ~data;
             status |= PORT_RWC & data;
             *reg    = status;
}

uint32_t
EHCI::ResetPort(uint32_t port) {
    volatile uint32_t *reg = &Ops->ports[port];
    PortSet(reg, PORT_RESET);
    for (int i = 0; i < 100; i++) { printf(""); }
    PortClear(reg, PORT_RESET);
    
    uint32_t status = 0;
    for (uint8_t i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; j++) { printf(""); }
        
        status = *reg;
        
        if (~status & PORT_CONNECTION) {
            break;
        }
        
        // ACK change in status
        if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE)) {
            PortClear(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
            continue;
        }
        
        // Check if device is enabled
        if (status & PORT_ENABLE) {
            break;
        }
    }
    return status;
}
