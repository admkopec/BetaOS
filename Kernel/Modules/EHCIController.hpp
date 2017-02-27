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
#include "PCIController.hpp"
#include "MMIOUtils.hpp"

#define PCI_USB_CLASS    0x0C
#define PCI_USB_SUBCLASS 0x03
#define PCI_USB_EHCI     0x20
#define MAX_QH              8
#define MAX_TD             32

typedef struct {
    uint8_t len;
    uint8_t type;
    uint16_t hidVer;
    uint8_t countryCode;
    uint8_t descCount;
    uint8_t descType;
    uint16_t descLen;
} __packed USBHidDesc;
typedef struct {
    uint8_t len;
    uint8_t type;
    uint8_t portCount;
    uint16_t chars;
    uint8_t portPowerTime;
    uint8_t current;
    // removable/power control bits vary in size
} __packed USBHubDesc;
typedef struct {
    uint8_t len;
    uint8_t type;
    uint8_t addr;
    uint8_t attributes;
    uint16_t maxPacketSize;
    uint8_t interval;
} __packed USBEndpointDesc;
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
} __packed USBDevReq;
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
    uint8_t  capLength;
    uint8_t  reserved;
    uint16_t hciVersion;
    uint32_t hcsParams;
    uint32_t hccParams;
    uint64_t hcspPortRoute;
} __packed CapRegisters;
typedef struct {
    volatile uint32_t USBCommad;
    volatile uint32_t USBStatus;
    volatile uint32_t USBInterrupt;
    volatile uint32_t frameIndex;
    volatile uint32_t ctrlDsSegment;
    volatile uint32_t periodicListBase;
    volatile uint32_t asyncListAddr;
    volatile uint32_t reserved[9];
    volatile uint32_t configFlag;
    volatile uint32_t ports[];
}          OpRegisters;

class EHCI {
    CapRegisters *Caps;
    OpRegisters  *Ops;
    uint32_t     *FrameList;
    EhciQH       *QHpool;
    EhciTD       *TDpool;
    EhciQH       *AsyncQH;
    EhciQH       *PeriodicQH;
    
    unsigned int ResetPort  (unsigned int port);
    void         PortSet    (volatile uint32_t *reg, uint32_t data);
    void         PortClear  (volatile uint32_t *reg, uint32_t data);
public:
    int  init(PCI *pci);
    void start();
};

#endif /* EHCIController_hpp */
