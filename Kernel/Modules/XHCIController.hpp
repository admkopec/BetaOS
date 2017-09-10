//
//  XHCIController.h
//  Kernel
//
//  Created by Adam Kopeć on 1/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef XHCIController_hpp
#define XHCIController_hpp

#include <stdio.h>
#include "Controller.hpp"

#define PCI_USB_CLASS    0x0C
#define PCI_USB_SUBCLASS 0x03
#define PCI_USB_XHCI     0x30

#define PCI_XHCI_INTEL_XHCC                 0x40U    /* Intel xHC System Bus Configuration Register */
#define PCI_XHCI_INTEL_XHCC_SWAXHCI                (1U  << 11)
#define PCI_XHCI_INTEL_XHCC_SWAXHCIP_SET(x) (((x) & 3U) << 12)
#define PCI_XHCI_INTEL_XUSB2PR              0xD0U    /* Intel USB2 Port Routing */
#define PCI_XHCI_INTEL_XUSB2PRM             0xD4U    /* Intel USB2 Port Routing Mask */
#define PCI_XHCI_INTEL_USB3_PSSEN           0xD8U    /* Intel USB3 Port SuperSpeed Enable */
#define PCI_XHCI_INTEL_USB3PRM              0xDCU    /* Intel USB3 Port Routing Mask */

#define USB_FULLSPEED  1
#define USB_LOWSPEED   2
#define USB_HIGHSPEED  3
#define USB_SUPERSPEED 4

#define kMaxRootPorts   30
#define kUSBMaxDevices  128

typedef struct {
    uint8_t  CapabilitiesLength;
    uint8_t  Reserved;
    uint16_t HCIVersion;
    uint32_t HCSParams1;
    uint32_t HCSParams2;
    uint32_t HCSParams3;
    uint32_t HCCParams1;
    uint32_t DoorbellOffset;
    uint32_t RuntimeRegSpaceOffset;
    uint32_t HCCParams2;
} __packed CapabilityRegisters;

typedef struct {
    uint32_t PortSC;
    uint32_t PortPmsc;
    uint32_t PortLi;
    uint32_t Reserved;
} Port;

typedef struct {
    uint32_t USBCommand;
    uint32_t USBStatus;
    uint32_t PageSize;
    uint32_t Reserved[2];
    uint32_t DeviceNotificationControl;
    uint16_t CommandRingControl;
    uint32_t Reserved_[4];
    uint64_t DeviceContextBaseAddressArrayPointer;
    uint32_t Configure;
    uint32_t Reserved__[241];
    Port     Ports[0];
} OperationalRegisters;

typedef struct {
    uint8_t CapabilityId;
    uint8_t Next;
    uint16_t CapabilitySpecific;
} ExtendedCapabilityRegisters;

typedef struct {
    uint8_t CapabilityId;
    uint8_t Next;
    uint8_t RevisionMinor;
    uint8_t RevisionMajor;
    uint32_t NameString;
    uint8_t CompatiblePortOffset;
    uint8_t CompatiblePortCount;
    uint16_t Reserved;
} ExtendedCapabilityRegisters_SP;

typedef struct {
    uint32_t Iman;
    uint32_t Imod;
    uint32_t Erstsz;
    uint32_t Reserved;
    uint64_t Erstba;
    uint64_t Erdp;
} InterruptRegisterSet;

typedef struct {
    uint32_t MFIndex;
    uint32_t Reserved[7];
    InterruptRegisterSet Interrupts[0];
} RuntimeRegisters;

#define	XHCI_HC_BIOS_OWNED	(1U << 16)
#define	XHCI_HC_OS_OWNED	(1U << 24)
#define	XHCI_LEGACY_DISABLE_SMI	((7U << 1) | (255U << 5) | (7U << 17))
#define	XHCI_LEGACY_SMI_EVENTS	(7U << 29)

typedef struct {
    uint32_t dwSctx0;
    uint32_t dwSctx1;
    uint32_t dwSctx2;
    uint32_t dwSctx3;
    uint32_t pad[4];
} SlotContext;

typedef struct {
    uint32_t dwEpCtx0;
    uint32_t dwEpCtx1;
    uint64_t qwEpCtx2;
    uint32_t dwEpCtx4;
    uint32_t pad[3];
} EndpointContext;

typedef struct {
    uint32_t dwInCtx0;
    uint32_t dwInCtx1;
    uint32_t pad[6];
} InputControlContext;

union DeviceContext{
    SlotContext     Slot;
    EndpointContext Endpoint;
    InputControlContext InputControl;
};

// Fix static declarations

class XHCI : public Controller {
    CapabilityRegisters*         Capabilities;
    ExtendedCapabilityRegisters* ExtendedCapabilities;
    OperationalRegisters*        Operationals;
    RuntimeRegisters*            Runtimes;
    uint32_t*                    DoorBells;
    DeviceContext                DeviceBaseArray[2];
    
    uint16_t    RootHub3Address;
    uint16_t    RootHub2Address;
    int         RootHubNumberOfPorts;
    int         NumberOfSlots;
    uint32_t    RootHubPortStatusChangeBitmap;
    bool        RootHubPortEmulateCSC[kMaxRootPorts];
    uint32_t*   USBLegacySupport;
    
    OSReturn  Handshake(volatile const uint32_t* pReg, uint32_t test_mask, uint32_t test_target, int32_t msec);
    void      DecodeSupportedProtocol(ExtendedCapabilityRegisters* pCap);
    void      TakeOwnershipFromBios(void);
    
public:
    virtual int  init(PCI *pci) override;
    virtual void start() override;
    virtual void stop() override;
};

#endif /* XHCIController_hpp */
