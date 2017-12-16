//
//  EHCIController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 1/13/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef EHCIController_hpp
#define EHCIController_hpp

#include <stdio.h>
#include <stdlib.h>
#include "Controller.hpp"
#include "USBCommon.hpp"

#define PCI_USB_CLASS    0x0C
#define PCI_USB_SUBCLASS 0x03
#define PCI_USB_EHCI     0x20
#define MAX_QH              8
#define MAX_TD             32

typedef struct Link {
    struct Link *prev;
    struct Link *next;
} Link;

typedef struct {
    uint8_t len;
    uint8_t type;
    uint16_t hidVer;
    uint8_t countryCode;
    uint8_t descCount;
    uint8_t descType;
    uint16_t descLen;
} __attribute__((packed)) USBHidDesc;
typedef struct {
    uint8_t len;
    uint8_t type;
    uint8_t portCount;
    uint16_t chars;
    uint8_t portPowerTime;
    uint8_t current;
    // removable/power control bits vary in size
} __attribute__((packed)) USBHubDesc;
typedef struct {
    uint8_t len;
    uint8_t type;
    uint8_t addr;
    uint8_t attributes;
    uint16_t maxPacketSize;
    uint8_t interval;
} __attribute__((packed)) USBEndpointDesc;
typedef struct {
    USBEndpointDesc desc;
    uint16_t        toggle;
}          USBEndpoint;
typedef struct {
    uint8_t  type;
    uint8_t  req;
    uint16_t value;
    uint16_t index;
    uint16_t len;
} __attribute__((packed)) USBDevReq;
typedef struct {
    USBEndpoint *endp;
    USBDevReq   *req;
    void        *data;
    uint16_t    len;
    bool        complete;
    bool        success;
}          USBTransfer;
typedef struct {
    volatile uint32_t link;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];
    
    // internal fields
    uint32_t tdNext;
    uint32_t active;
    uint8_t  pad[4];
}          EhciTD;
typedef struct {
    uint32_t          QueueHeadHorizontalLinkPointer;
    uint32_t          EndpointCharacteristics;
    uint32_t          EndpointCapabilities;
    volatile uint32_t curLink;
    
    // matches a transfer descriptor
    volatile uint32_t nextLink;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];
    
    // internal fields
    USBTransfer *transfer;
    Link        qhLink;
    uint32_t    tdHead;
    uint32_t    active;
    uint8_t     pad[20];
}          EhciQH;
typedef struct {
    uint8_t  CapabilitiesLength;
    uint8_t  Reserved;
    uint16_t HCIVersion;
    uint32_t HCSParams;
    uint32_t HCCParams;
    uint64_t HCSPPortRoute;
} __attribute__((packed)) CapRegisters;
typedef struct {
    volatile uint32_t USBCommand;
    volatile uint32_t USBStatus;
    volatile uint32_t USBInterrupt;
    volatile uint32_t FrameIndex;
    volatile uint32_t ControlDSSegment;
    volatile uint32_t PeriodicListBase;
    volatile uint32_t AsyncListAddress;
    volatile uint32_t Reserved[9];
    volatile uint32_t ConfigFlag;
    volatile uint32_t Ports[];
}         OpRegisters;

// Fix static declarations;

class EHCI : public Controller {
    CapRegisters*    CapabilityRegisters;
    OpRegisters*     OperationalRegisters;
    uint32_t*        FrameList;
    EhciQH*          QHpool;
    EhciTD*          TDpool;
    EhciQH*          AsyncQH;
    EhciQH*          PeriodicQH;
    
    uint32_t Vendor;
    uint32_t Device;
    
    OSReturn     Handshake  (volatile const uint32_t* pReg, uint32_t test_mask, uint32_t test_target, int32_t msec);
    void         ResetPort  (unsigned int port);
    void         PortSet    (volatile uint32_t *reg, uint32_t data);
    void         PortClear  (volatile uint32_t *reg, uint32_t data);
    void         LineStatusCheck(uint32_t port);
public:
    virtual int  init(PCI *pci) override;
    virtual void start() override;
    virtual void stop() override;
    virtual void handleInterrupt() override;
};

#endif /* EHCIController_hpp */
