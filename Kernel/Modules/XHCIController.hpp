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
#include "USBCommon.hpp"

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
} __attribute__((packed)) CapabilityRegisters;

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

//typedef struct {
//    uint32_t dwSctx0;
//    uint32_t dwSctx1;
//    uint32_t dwSctx2;
//    uint32_t dwSctx3;
//    uint32_t pad[4];
//} SlotContext;

//typedef struct {
//    uint32_t dwEpCtx0;
//    uint32_t dwEpCtx1;
//    uint64_t qwEpCtx2;
//    uint32_t dwEpCtx4;
//    uint32_t pad[3];
//} EndpointContext;

//typedef struct {
//    uint32_t dwInCtx0;
//    uint32_t dwInCtx1;
//    uint32_t pad[6];
//} InputControlContext;

struct SlotContext {
    volatile uint32_t RouteString       : 20;
    volatile uint32_t Speed             :  4;
    volatile uint32_t Reserved1         :  1;
    volatile uint32_t MTT               :  1;
    volatile uint32_t Hub               :  1;
    volatile uint32_t ContextEntries    :  5;
    
    volatile uint16_t MaxExitLatency;
    volatile uint8_t  RootHubPortNumber;
    volatile uint8_t  NumberOfPorts;
    
    volatile uint8_t  TTHubSlotID;
    volatile uint8_t  TTPortNumber;
    volatile uint32_t TTT               :  2;
    volatile uint32_t Reserved2         :  4;
    volatile uint32_t InterruptTarget   : 10;
    
    volatile uint8_t  USBDeviceAddress;
    volatile uint32_t Reserved3         : 19;
    volatile uint32_t SlotState         :  5;
    
    volatile uint32_t Reserved4[4];
};

struct EndpointContext {
    volatile uint32_t EndpointState     :  3;
    volatile uint32_t Reserved1         :  5;
    volatile uint32_t Mult              :  2;
    volatile uint32_t MaxPStreams       :  5;
    volatile uint32_t Lsa               :  1;
    volatile uint8_t  Interval;
    volatile uint8_t  Reserved2;
    
    volatile uint32_t Reserved3         :  1;
    volatile uint32_t ErrorCount        :  2;
    volatile uint32_t EndpointType      :  3;
    volatile uint32_t Reserved4         :  1;
    volatile uint32_t HID               :  1;
    volatile uint8_t  MaxBurstSize;
    volatile uint16_t MaxPacketSize;
    
    volatile uint32_t DCS               :  1;
    volatile uint32_t Reserved5         :  3;
    volatile uint32_t TRdequeuePtrLo    : 28;
    
    volatile uint32_t TRdequeuePtrHi;
    
    volatile uint16_t AverageTRBLenghth;
    volatile uint16_t MaxESITPayload;
    
    volatile uint32_t Reserved6[3];
};

struct InputControlContext {
    volatile uint32_t D; // Drop Context flags. D0 and D1 are reserved.
    volatile uint32_t A; // Add Context flags
    
    uint32_t Reserved1[5];
    
    uint8_t ConfigValue;
    uint8_t InterfaceNumber;
    uint8_t AlternateSetting;
    uint8_t Reserved2;
};

struct DeviceContextArray {
    volatile uint64_t ScratchpadBufferArrayBase;
    volatile uint64_t DeviceContextPointer[kMaxRootPorts];
};

struct DeviceContext {
    SlotContext     Slot;
    EndpointContext Endpoints[31];
};

struct InputContext {
    InputControlContext ICC;
    DeviceContext       DC;
};

struct CommandDeviceTRB {
    uint32_t InputContextPtrLo;      // Bit 0-3 RsvdZ - aligned on a 16-byte address boundary
    uint32_t InputContextPtrHi;
    
    uint32_t Reserved1;              // RsvdZ
    
    uint32_t Cycle             :  1;
    uint32_t Reserved2         :  8; // RsvdZ
    uint32_t BSR_DC            :  1;
    uint32_t TRBtype           :  6;
    uint32_t Reserved3         :  8; // RsvdZ
    uint8_t  SlotID;
};

struct LinkTRB {
    uint32_t RingSegmentPtrLo;       // only high order valid! - must be aligned on a 16-byte address boundary
    uint32_t RingSegmentPtrHi;
    
    uint32_t Reserved1         : 22; // RsvdZ
    uint32_t IntTarget         : 10; // Interrupter Target
    
    uint32_t Cycle             :  1;
    uint32_t TC                :  1;
    uint32_t Reserved2         :  2; // RsvdZ
    uint32_t CH                :  1;
    uint32_t IOC               :  1;
    uint32_t Reserved3         :  4; // RsvdZ
    uint32_t TRBtype           :  6;
    uint32_t Reserved4         : 16; // RsvdZ
};

struct EventTRB {
    uint32_t EventDataLo;            // physical memory pointer
    uint32_t EventDataHi;
    
    uint32_t TransferLength    : 24; // only Transfer Event
    uint8_t  CompletionCode;
    
    uint32_t Cycle             :  1;
    uint32_t Reserved1         :  1;
    uint32_t ED                :  1; // only Transfer Event
    uint32_t Reserved2         :  7;
    uint32_t TRBtype           :  6;
    uint8_t  Byte3;                  // Transfer Event: Endpoint (16-20); Cmd Compl., Doorbell: VF (ID of the Virtual Function that generated the event)
    uint8_t  Slot;                   // Transfer Event, Cmd Compl., Bandwidth Request, Doorbell, Device Notification
};

struct Xfer_NormalTRB {
    uint32_t DataBufferPtrLo;
    uint32_t DataBufferPtrHi;
    
    uint32_t TransferLength    : 17;
    uint32_t TDsize            :  5; // indicator of the number of packets remaining in the TD, cf. 4.11.2.4
    uint32_t IntTarget         : 10; // Interrupter Target
    
    uint32_t Cycle             :  1;
    uint32_t ENT               :  1; // Evaluate Next TRB
    uint32_t ISP               :  1; // Interrupt-on Short Packet
    uint32_t NS                :  1; // No Snoop
    uint32_t CH                :  1; // Chain bit
    uint32_t IOC               :  1; // Interrupt On Completion
    uint32_t IDT               :  1; // Immediate Data
    uint32_t Reserved1         :  2;
    uint32_t BEI               :  1; // Block Event Interrupt
    uint32_t TRBtype           :  6; // Normal: ID 1, cf. Table 131: TRB Type Definitions
    uint32_t Reserved2         :  16;
};

struct Xfer_SetupStageTRB {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    
    uint16_t wIndex;
    uint16_t wLength;
    
    uint32_t TransferLength    : 17;
    uint32_t Reserved1         :  5;
    uint32_t IntTarget         : 10;
    
    uint32_t Cycle             :  1;
    uint32_t Reserved2         :  4;
    uint32_t IOC               :  1;
    uint32_t IDT               :  1;
    uint32_t Reserved3         :  3;
    uint32_t TRBtype           :  6; // Setup Stage: ID 2, cf. Table 131: TRB Type Definitions
    uint32_t TRT               :  2; // Transfer Type
    uint32_t Reserved4         :  14;
};

struct Xfer_DataStageTRB {
    uint32_t DataBufferPtrLo;
    uint32_t DataBufferPtrHi;
    
    uint32_t TransferLength    : 17;
    uint32_t TDsize            :  5; // indicator of the number of packets remaining in the TD, cf. 4.11.2.4
    uint32_t IntTarget         : 10; // Interrupter Target
    
    uint32_t Cycle             :  1;
    uint32_t ENT               :  1; // Evaluate Next TRB
    uint32_t ISP               :  1; // Interrupt-on Short Packet
    uint32_t NS                :  1; // No Snoop
    uint32_t CH                :  1; // Chain bit
    uint32_t IOC               :  1; // Interrupt On Completion
    uint32_t IDT               :  1; // Immediate Data
    uint32_t Reserved1         :  3;
    uint32_t TRBtype           :  6; // Data Stage: ID 3, cf. Table 131: TRB Type Definitions
    uint32_t DIR               :  1; // Direction
    uint32_t Reserved2         :  15;
} ;

struct Xfer_StatusStageTRB {
    uint32_t Reserved1;
    uint32_t Reserved2;
    
    uint32_t Reserved3         : 22;
    uint32_t IntTarget         : 10; // Interrupter Target
    
    uint32_t Cycle             :  1;
    uint32_t ENT               :  1; // Evaluate Next TRB
    uint32_t Reserved4         :  2;
    uint32_t CH                :  1; // Chain bit
    uint32_t IOC               :  1; // Interrupt On Completion
    uint32_t Reserved5         :  4;
    uint32_t TRBtype           :  6; // Status Stage: ID 4, cf. Table 131: TRB Type Definitions
    uint32_t DIR               :  1; // Direction
    uint32_t Reserved6         :  15;
};

struct Endpoint {
    uint32_t  TimeTransfer;
    uint32_t  TimeEvent;
    uint8_t   EndpointState;
    bool      PendingTransfer;  // 0: event occurred  1: waiting for event
    uint8_t   TransferError;    // cf. 6.4.5 TRB Completion Codes
    
    // transfer ring
    bool                   TransferRingProducerCycleState; // PCS
    Xfer_NormalTRB*        EnqTransferRingVirtPointer;
    Xfer_NormalTRB*        DeqTransferRingVirtPointer;
    Xfer_NormalTRB*        TransferRingbase;
    uint32_t               TransferCounter;
};

struct Slot {
    Endpoint Endpoints[31];
    uint8_t  SlotState;
};

struct PortSlotLink {
    uint8_t  SlotNumber;
    LinkTRB* CommandPointer;
};

class XHCI : public Controller, protected USBHostController {
    CapabilityRegisters*         Capabilities;
    ExtendedCapabilityRegisters* ExtendedCapabilities;
    OperationalRegisters*        Operationals;
    RuntimeRegisters*            Runtimes;
    uint32_t*                    DoorBells;
    
    DeviceContextArray* DeviceContextArrayBaseVirtPointer;
    DeviceContext*      DeviceContextPointer[kMaxRootSlots];
    InputContext*       DeviceInputContextPointer[kMaxRootSlots];
    
    LinkTRB* EnqCommandRingVirtPointer;
    LinkTRB* CommandRingBase;
    uint32_t CommandCounter;
    uint32_t CommandPending;
    
    EventTRB* DeqEventRingVirtPointer;
    EventTRB* EventRingBase;
    uint32_t  EventCounter;
    uint32_t  EventSegmentSize;
    uint32_t  EventNumberOfSegments;
    
    Xfer_NormalTRB*  TransferRings[kMaxRootSlots][31];
    
    bool CommandRingProducerCycleState; // PCS
    bool EventRingConsumerCycleState; // CCS
    
    bool GotIRQ;
    
    uint16_t    RootHub3Address;
    uint16_t    RootHub2Address;
    int         NumberOfSlots;
    uint32_t    RootHubPortStatusChangeBitmap;
    bool        RootHubPortEmulateCSC[kMaxRootPorts];
    uint32_t*   USBLegacySupport;
    
    OSReturn  Handshake(volatile const uint32_t* pReg, uint32_t test_mask, uint32_t test_target, int32_t msec);
    void      DecodeSupportedProtocol(ExtendedCapabilityRegisters* pCap);
    void      TakeOwnershipFromBios(void);
    void      ParseEvents(void);
    void      AddressDeviceCommand(uint8_t SlotNumber, bool BSR);
    void      RingDoorbellForDevice(uint8_t SlotNumber, uint8_t Target, uint16_t StreamID);
    void      RingDoorbellForHostAndWait(void);
    void      SetEnqueueTransferPtr(uint8_t SlotNumber, uint8_t DCI);
    LinkTRB*  EnqueueCommand(LinkTRB* pCommand);
    void      SetEnqueueCommandPtr(void);
    Xfer_NormalTRB* EnqueueTransfer(Xfer_NormalTRB* pTransfer, uint8_t SlotNumber, uint8_t DCI);
public:
    Slot*        Slots[kMaxRootSlots];
    bool         PortsEnabled[kMaxRootPorts];
    PortSlotLink PortSlotLinks[kMaxRootPorts];
    virtual uint8_t      SetupTransaction(Transfer* transfer, Transaction* transaction, bool toggle, uint8_t type, uint8_t Request, uint8_t HiVal, uint8_t LoVal, uint16_t Index, uint16_t Length) override;
    virtual void InTransaction(Transfer* transfer, Transaction* transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingIn) override;
    virtual void OutTransaction(Transfer* transfer, Transaction* transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingOut) override;
    virtual void ScheduleTransfer(Transfer* transfer) override;
    virtual void WaitForTransfer(Transfer* transfer) override;
    virtual int  init(PCI *pci) override;
    virtual void start() override;
    virtual void stop() override;
    virtual void handleInterrupt() override;
};

#endif /* XHCIController_hpp */
