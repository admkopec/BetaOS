//
//  XHCIController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 1/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "XHCIController.hpp"
#include "InterruptController.hpp"

#define super Controller

// USB Command Register

#define CMD_RUN                         (1  <<  0)      // Run/Stop
#define CMD_HCRESET                     (1  <<  1)      // Host Controller Reset
#define CMD_INTE                        (1  <<  2)      // Interrupt Enable
#define CMD_HSEE                        (1  <<  3)      // Host System Error Enable
#define CMD_LHCRESET                    (1  <<  7)      // Light Host Controller Reset
#define CMD_CSS                         (1  <<  8)      // Controller Save State
#define CMD_CRS                         (1  <<  9)      // Controller Restore State
#define CMD_EWE                         (1  << 10)      // Enable Wrap Event
#define CMD_EU3S                        (1  << 11)      // Enable U3 MFINDEX Stop
#define CMD_SPE                         (1  << 12)      // Stopped - Short Packet Enable (Reserved if SPC == 0)
#define CMD_CME                         (1  << 13)      // CEM Enable

// USB Status Register

#define STS_HALT                        (1  <<  0)      // Host Controller Halted
#define STS_HSE                         (1  <<  2)      // Host System Error
#define STS_EINT                        (1  <<  3)      // Event Interrupt
#define	STS_CNR                         0x00000800U     // Controller Not Ready


// USB Port Register

#define	PS_CCS              0x00000001U             // RO - current connect status 
#define	PS_PED              0x00000002U             // RW1C - port enabled / disabled 
#define	PS_OCA              0x00000008U             // RO - over current active 
#define	PS_PR               0x00000010U             // RW1S - port reset 
#define	PS_PLS_GET(x)       (((x) >> 5) & 0xFU)     // RW - port link state 
#define	PS_PLS_SET(x)       (((x) & 0xFU) << 5)     // RW - port link state 
#define	PS_PP               0x00000200U             // RW - port power 
#define	PS_SPEED_GET(x)     (((x) >> 10) & 0xFU)	// RO - port speed 
#define	PS_PIC_GET(x)       (((x) >> 14) & 0x3U)	// RW - port indicator 
#define	PS_PIC_SET(x)       (((x) & 0x3U) << 14)	// RW - port indicator 
#define	PS_LWS              0x00010000U             // RW1S - port link state write strobe 
#define	PS_CSC              0x00020000U             // RW1C - connect status change 
#define	PS_PEC              0x00040000U             // RW1C - port enable/disable change 
#define	PS_WRC              0x00080000U             // RW1C - warm port reset change (RsvdZ for USB2 ports) 
#define	PS_OCC              0x00100000U             // RW1C - over-current change 
#define	PS_PRC              0x00200000U             // RW1C - port reset change 
#define	PS_PLC              0x00400000U             // RW1C - port link state change 
#define	PS_CEC              0x00800000U             // RW1C - config error change (RsvdZ for USB2 ports) 
#define	PS_CAS              0x01000000U             // RO - cold attach status 
#define	PS_WCE              0x02000000U             // RW - wake on connect enable 
#define	PS_WDE              0x04000000U             // RW - wake on disconnect enable 
#define	PS_WOE              0x08000000U             // RW - wake on over-current enable 
#define	PS_DR               0x40000000U             // RO - device removable
#define	PS_WPR              0x80000000U             // RW1S - warm port reset (RsvdZ for USB2 ports)
#define	PS_CLEAR            0x80FF01FFU             // command bits
#define	PS_WAKEBITS         (PS_WCE | PS_WDE | PS_WOE)
#define	PS_WRITEBACK_MASK   (PS_PP | PS_PIC_SET(3U) | PS_WAKEBITS)

// USB Capability Register

#define	HCS1_DEVSLOT_MAX(x)  ((x) &  0xFFU)
#define	HCS1_IRQ_MAX(x)     (((x) >> 8)  & 0x7FFU)
#define	HCS1_N_PORTS(x)     (((x) >> 24) & 0xFFU)
#define	HCS2_IST(x)          ((x) &  0xFU)
#define	HCS2_ERST_MAX(x)	(((x) >> 4)  & 0xFU)
#define	HCS2_SPB_MAX(x)     (((x) >> 27) & 0x1FU)
#define	HCC_AC64(x)          ((x) &  0x1U)          // 64-bit capable
#define	HCC_BNC(x)          (((x) >> 1)  & 0x1U)    // BW negotiation
#define	HCC_CSZ(x)          (((x) >> 2)  & 0x1U)    // context size
#define	HCC_PPC(x)          (((x) >> 3)  & 0x1U)    // port power control
#define	HCC_PIND(x)         (((x) >> 4)  & 0x1U)    // port indicators
#define	HCC_LHRC(x)         (((x) >> 5)  & 0x1U)    // light HC reset
#define	HCC_LTC(x)          (((x) >> 6)  & 0x1U)    // latency tolerance msg
#define	HCC_NSS(x)          (((x) >> 7)  & 0x1U)    // no secondary sid
#define	HCC_PSA_SZ_MAX(x)	(((x) >> 12) & 0xFU)	// max pri. stream array size
#define	HCC_XECP(x)         (((x) >> 16) & 0xFFFFU)	// extended capabilities pointer

// USB Slot Context

#define SCTX_0_ROUTE_SET(x)        ((x)  & 0xFFFFF)
#define SCTX_0_ROUTE_GET(x)        ((x)  & 0xFFFFF)
#define SCTX_0_SPEED_SET(x)        (((x) & 0xF) << 20)
#define SCTX_0_SPEED_GET(x)        (((x) >> 20) & 0xF)
#define SCTX_0_MTT_SET(x)          (((x) & 0x1) << 25)
#define SCTX_0_MTT_GET(x)          (((x) >> 25) & 0x1)
#define SCTX_0_HUB_SET(x)          (((x) & 0x1) << 26)
#define SCTX_0_HUB_GET(x)          (((x) >> 26) & 0x1)
#define SCTX_0_CTX_NUM_SET(x)      (((x) & 0x1F) << 27)
#define SCTX_0_CTX_NUM_GET(x)      (((x) >> 27) & 0x1F)
#define SCTX_1_MAX_EL_SET(x)        ((x)  & 0xFFFF)
#define SCTX_1_MAX_EL_GET(x)        ((x)  & 0xFFFF)
#define SCTX_1_RH_PORT_SET(x)       (((x) & 0xFF) << 16)
#define SCTX_1_RH_PORT_GET(x)       (((x) >> 16) & 0xFF)
#define SCTX_1_NUM_PORTS_SET(x)     (((x) & 0xFF) << 24)
#define SCTX_1_NUM_PORTS_GET(x)     (((x) >> 24) & 0xFF)
#define SCTX_2_TT_HUB_SID_SET(x)     ((x) & 0xFF)
#define SCTX_2_TT_HUB_SID_GET(x)     ((x) & 0xFF)
#define SCTX_2_TT_PORT_NUM_SET(x)   (((x) & 0xFF) << 8)
#define SCTX_2_TT_PORT_NUM_GET(x)   (((x) >> 8) & 0xFF)
#define SCTX_2_TT_THINK_TIME_SET(x) (((x) & 0x3) << 16)
#define SCTX_2_TT_THINK_TIME_GET(x) (((x) >> 16) & 0x3)
#define SCTX_2_IRQ_TARGET_SET(x)    (((x) & 0x3FF) << 22)
#define SCTX_2_IRQ_TARGET_GET(x)     ((x) >> 22) & 0x3FF)
#define SCTX_3_DEV_ADDR_SET(x)       ((x) & 0xFF)
#define SCTX_3_DEV_ADDR_GET(x)       ((x) & 0xFF)
#define SCTX_3_SLOT_STATE_SET(x)    (((x) & 0x1F) << 27)
#define SCTX_3_SLOT_STATE_GET(x)     ((x) >> 27) & 0x1F)

// USB Endpoint Context

#define EPCTX_0_EPSTATE_SET(x)           ((x) & 0x7)
#define EPCTX_0_EPSTATE_GET(x)           ((x) & 0x7)
#define EPCTX_0_MULT_SET(x)             (((x) & 0x3) << 8)
#define EPCTX_0_MULT_GET(x)             (((x) >> 8) & 0x3)
#define EPCTX_0_MAXP_STREAMS_SET(x)     (((x) & 0x1F) << 10)
#define EPCTX_0_MAXP_STREAMS_GET(x)     (((x) >> 10) & 0x1F)
#define EPCTX_0_LSA_SET(x)              (((x) & 0x1) << 15)
#define EPCTX_0_LSA_GET(x)              (((x) >> 15) & 0x1)
#define EPCTX_0_IVAL_SET(x)             (((x) & 0xFF) << 16)
#define EPCTX_0_IVAL_GET(x)             (((x) >> 16) & 0xFF)
#define EPCTX_1_CERR_SET(x)             (((x) & 0x3) << 1)
#define EPCTX_1_CERR_GET(x)             (((x) >> 1) & 0x3)
#define EPCTX_1_EPTYPE_SET(x)           (((x) & 0x7) << 3)
#define EPCTX_1_EPTYPE_GET(x)           (((x) >> 3) & 0x7)
#define EPCTX_1_HID_SET(x)              (((x) & 0x1) << 7)
#define EPCTX_1_HID_GET(x)              (((x) >> 7) & 0x1)
#define EPCTX_1_MAXB_SET(x)             (((x) & 0xFF) << 8)
#define EPCTX_1_MAXB_GET(x)             (((x) >> 8) & 0xFF)
#define EPCTX_1_MAXP_SIZE_SET(x)        (((x) & 0xFFFF) << 16)
#define EPCTX_1_MAXP_SIZE_GET(x)        (((x) >> 16) & 0xFFFF)
#define EPCTX_2_DCS_SET(x)               ((x) & 0x1)
#define EPCTX_2_DCS_GET(x)               ((x) & 0x1)
#define EPCTX_2_TR_DQ_PTR_MASK           0xFFFFFFFFFFFFFFF0ULL
#define EPCTX_4_AVG_TRB_LEN_SET(x)       ((x) & 0xFFFF)
#define EPCTX_4_AVG_TRB_LEN_GET(x)       ((x) & 0xFFFF)
#define EPCTX_4_MAX_ESIT_PAYLOAD_SET(x) (((x) & 0xFFFF) << 16)
#define EPCTX_4_MAX_ESIT_PAYLOAD_GET(x) (((x) >> 16) & 0xFFFF)

// USB Input Controller Context

#define INCTX_NON_CTRL_MASK    0xFFFFFFFCU
#define INCTX_0_DROP_MASK(n)   (1U << (n))
#define INCTX_1_ADD_MASK(n)    (1U << (n))

#define CONFIG_SLOTS_MASK	0x000000FFU

#define GetPortSC(x) (x & (PS_DR | PS_WAKEBITS | PS_CAS |PS_PIC_SET(3U) | (15U << 10) /* Speed */ | PS_PP | PS_OCA | PS_CCS))

#define Log(x ...) printf("XHCIController: " x)
#ifdef DEBUG
#define DBG(x ...) printf("XHCIController: " x)
#else
#define DBG(x ...)
#endif

OSReturn
XHCI::init(PCI *pci) {
    if (!(pci->ClassCode() == PCI_USB_CLASS && pci->SubClass() == PCI_USB_SUBCLASS && pci->ProgIF() == PCI_USB_XHCI)) {
        return kOSReturnFailed;
    }
    
    Log("XHCI found!\n");
    Log("Vendor: %x Device: %x\n", pci->VendorID(), pci->DeviceID());
    
    pci->getBAR(0);
    pci->EnableBusMastering();
    
    RootHubPortStatusChangeBitmap = 0;
    RootHub3Address = 128;
    RootHub2Address = 129;
    
    Capabilities = (CapabilityRegisters*) (pci->BAR().u.address);
    Operationals = (OperationalRegisters*)(pci->BAR().u.address + Capabilities->CapabilitiesLength);
    Runtimes     = (RuntimeRegisters*)    (pci->BAR().u.address + Capabilities->RuntimeRegSpaceOffset);
    DoorBells    = (uint32_t*)            (pci->BAR().u.address + Capabilities->DoorbellOffset);
    
    NumberOfSlots        = (int)HCS1_DEVSLOT_MAX(Capabilities->HCSParams1);
    RootHubNumberOfPorts = (int)HCS1_N_PORTS(Capabilities->HCSParams1);
    if (!RootHubNumberOfPorts || RootHubNumberOfPorts > kMaxRootPorts) {
        Log("Invalid number of Root Hub ports = %d\n", RootHubNumberOfPorts);
        return kOSReturnError;
    }
    
    DBG("Number of Root Hub ports = %d\n", RootHubNumberOfPorts);
    
    Interrupt::Register(pci->IntLine(), NULL);
    
    uint32_t ecp = HCC_XECP(Capabilities->HCCParams1);
    if (!ecp) {
        Log("No ECP!\n");
        return  kOSReturnError;
    }
    
    ExtendedCapabilities = (ExtendedCapabilityRegisters*)((uint32_t*)(Capabilities) + ecp);
    ExtendedCapabilityRegisters* iterator = ExtendedCapabilities;
    while (true) {
        if (iterator->CapabilityId == 1) {
            USBLegacySupport = (uint32_t*)(iterator);
        } else if (iterator->CapabilityId == 2) {
            DecodeSupportedProtocol(iterator);
        }
        if (!(iterator->Next)) {
            break;
        }
        iterator = (ExtendedCapabilityRegisters*)((uint32_t*)(iterator) + iterator->Next);
    }
    
    pci->Write32(0xD8, 0xFFFFFFFF);
    pci->Write32(0xD0, 0xFFFFFFFF);
    
    TakeOwnershipFromBios();
    if (pci->VendorID() == 0x8086) {
        uint32_t v1, v2, v3, v4;
        v1 = pci->Read32(PCI_XHCI_INTEL_XUSB2PR);
        v2 = pci->Read32(PCI_XHCI_INTEL_XUSB2PRM);
        v3 = pci->Read32(PCI_XHCI_INTEL_USB3_PSSEN);
        v4 = pci->Read32(PCI_XHCI_INTEL_USB3PRM);
        if (v1 == UINT32_MAX || v2 == UINT32_MAX || v3 == UINT32_MAX || v4 == UINT32_MAX) {
            Log("Error Invalid Regspace for Intel XHCI!\n");
            return kOSReturnError;
        }
        v1 &= ~v2;
        v1 |= v2;
        pci->Write32(PCI_XHCI_INTEL_XUSB2PR, v1);
        pci->Write32(PCI_XHCI_INTEL_USB3_PSSEN, (v3 & ~v4) | v4);
    }
    
    NameString = (char*)"XHCIController (USB 3.0)";
    Used_ = true;
    return kOSReturnSuccess;
}

void
XHCI::start() {
    Log("Starting...\n");
    
    OSReturn status;
    if (Operationals->USBStatus == ~(uint32_t)0) {
        Log("Card Removed\n");
        return ;
    }
    
    status = Handshake(&Operationals->USBStatus, STS_CNR, 0, 1000);
    if (status != kOSReturnSuccess) {
        Log("Handshake Error Code = %x\n", status);
        return ;
    }
    
    Operationals->Configure = ((Operationals->Configure & ~CONFIG_SLOTS_MASK) | NumberOfSlots);
    Operationals->DeviceNotificationControl = UINT16_MAX;
    
    Operationals->DeviceContextBaseAddressArrayPointer = (uint64_t)&DeviceBaseArray[0];
    //Operationals->DeviceContextBaseAddressArrayPointer = (uint64_t)DeviceBaseArray[0];
    
    Operationals->USBCommand |= CMD_HCRESET;
    
    Operationals->USBCommand = 0;
    
    status = Handshake(&Operationals->USBStatus, STS_HALT, UINT32_MAX, 100);
    if (status != kOSReturnSuccess) {
        Log("Handshake Error Code = %x\n", status);
        return ;
    }
    
    Operationals->USBCommand |= CMD_RUN;
    
    status = Handshake(&Operationals->USBStatus, STS_HALT, 0, 1000);
    if (status != kOSReturnSuccess) {
        Log("%s on Run Host!\n", OSReturnStrings[status]);
        return ;
    }
    
    for (uint8_t port = 0; port < RootHubNumberOfPorts; ++port) {   // Port Reset for 2.0 Devices (Low Spped, Full Speed, High Speed)
        if (Operationals->Ports[port].PortSC == ~(uint32_t)0) {
            Log("No Device\n");
            return ;
        }
        if (Operationals->Ports[port].PortSC & PS_WRC)
            Operationals->Ports[port].PortSC = (Operationals->Ports[port].PortSC & PS_WRITEBACK_MASK) | PS_WRC;
        if ((Operationals->Ports[port].PortSC & (PS_CSC | PS_CCS)) == PS_CCS) {
            RootHubPortEmulateCSC[port] = true;
            static_cast<void>(__sync_fetch_and_or(&RootHubPortStatusChangeBitmap, (2U << port)));
        }
        printf("");
        uint32_t portSC = GetPortSC(Operationals->Ports[port].PortSC);
        portSC |= PS_PP;
        Operationals->Ports[port].PortSC = portSC;
        status = Handshake(&Operationals->Ports[port].PortSC, PS_PP, portSC, 2000);
        if(status != kOSReturnSuccess) {
            Log("Failed powering up port %d, error (%s)", port, OSReturnStrings[status]);
        }
        if (Operationals->Ports[port].PortSC & PS_PP) {
            Operationals->Ports[port].PortSC = GetPortSC(Operationals->Ports[port].PortSC) | PS_WPR;
            for (int i = 0; i < 2000; i++) {
                if (Operationals->Ports[port].PortSC & PS_PRC) {
                    break;
                }
                printf("");
            }
            Operationals->Ports[port].PortSC |= PS_PLS_SET(0);
            if (Operationals->Ports[port].PortSC & PS_PED) {
                Log("Port (%d) is in enabled state! Speed: ", port);
                int speed = PS_SPEED_GET(GetPortSC(Operationals->Ports[port].PortSC));
                switch (speed) {
                    case USB_LOWSPEED: //case USB_FULLSPEED:
                        printf("USB 1.0\n");
                        break;
                    case USB_FULLSPEED:
                        printf("USB 1.1\n");
                        break;
                    case USB_HIGHSPEED:
                        printf("USB 2.0\n");
                        break;
                    case USB_SUPERSPEED: default:
                        printf("USB 3.0\n");
                        break;
                        // You should set up a device/host here, but first you need to have a working transfer
                }
            }
        } else {
            Operationals->Ports[port].PortSC |= PS_PP;
            for (int i = 0; i < 2000; i++) {
                printf("");
            }
            if ((Operationals->Ports[port].PortSC & PS_PP) != 1) {
                Log("Failed powering up %d port\n", port);
            }
        }
        DBG("Status: %x\n", Operationals->Ports[port].PortSC);
    }
    Operationals->Configure = ((Operationals->Configure & ~CONFIG_SLOTS_MASK) | NumberOfSlots);
    Operationals->DeviceNotificationControl = UINT16_MAX;
}

void
XHCI::stop() {
    Log("Stopping...\n");
    if (!(Operationals->USBStatus & STS_HALT)) {
        Operationals->USBCommand &= ~CMD_RUN;
    }
    
    OSReturn status = Handshake(&Operationals->USBStatus, STS_HALT, STS_HALT, 1000);
    if (status != kOSReturnSuccess) {
        Log("%s on Stop Host!\n", OSReturnStrings[status]);
    }
    super::stop();
}

OSReturn
XHCI::Handshake(volatile const uint32_t* pReg, uint32_t test_mask, uint32_t test_target, int32_t msec) {
    for (int32_t count = 0; count < msec; ++count) {
        //if (count)
        //    printf("");
        //OSSleep(1U);
        if (*pReg == ~(uint32_t)0) {
            Log("Device Removed\n");
            return kOSReturnError;
        }
        if ((*pReg & test_mask) == (test_target & test_mask))
            return kOSReturnSuccess;
        printf("");
    }
    return kOSReturnTimeout;
}

void
XHCI::TakeOwnershipFromBios(void) {
    uint32_t v;
    int rc;
    
    if (!USBLegacySupport)
        return;
    if (*USBLegacySupport == ~(uint32_t)0) {
        Log("Device Removed\n");
        return;
    }
    if (*USBLegacySupport & XHCI_HC_BIOS_OWNED) {
        *USBLegacySupport |= XHCI_HC_OS_OWNED;
        rc = Handshake(USBLegacySupport, XHCI_HC_BIOS_OWNED, 0U, 100);
        if (rc == -3)
            return;
        if (rc == false) {
            Log("Unable to take ownership of Host Controller from BIOS within 100 ms\n");
            /*
             * Fall through to break bios hold by disabling SMI enables
             */
        }
    }
    v = *(USBLegacySupport + 1);
    if (v == ~(uint32_t)0)
        return;
    /*
     * Clear all SMI enables
     */
    v &= XHCI_LEGACY_DISABLE_SMI;
    /*
     * Clear RW1C bits
     */
    v |= XHCI_LEGACY_SMI_EVENTS;
    *(USBLegacySupport + 1) = v;
}

void
XHCI::DecodeSupportedProtocol(ExtendedCapabilityRegisters* pCap) {
    ExtendedCapabilityRegisters_SP * pSPCap = (ExtendedCapabilityRegisters_SP*)(pCap);
    switch (pSPCap->RevisionMajor) {
        case 3U:
            if (pSPCap->NameString != ' BSU')
                break;
            //_v3ExpansionData->_rootHubNumPortsSS = pSPCap->CompatiblePortCount;
            //_v3ExpansionData->_rootHubPortsSSStartRange = pSPCap->CompatiblePortOffset;
            break;
        case 2U:
            if (pSPCap->NameString != ' BSU')
                break;
            //_v3ExpansionData->_rootHubNumPortsHS = pSPCap->CompatiblePortCount;
            //_v3ExpansionData->_rootHubPortsHSStartRange = pSPCap->CompatiblePortOffset;
            break;
    }
}
