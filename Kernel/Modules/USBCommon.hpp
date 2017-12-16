//
//  USBCommon.hpp
//  Kernel
//
//  Created by Adam Kopeć on 11/24/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef USBCommon_hpp
#define USBCommon_hpp

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// double linked list element
typedef struct dlelement dlelement_t;
struct dlelement {
    void*        data;
    dlelement_t* prev;
    dlelement_t* next;
};

typedef struct {
    dlelement_t* head;
    dlelement_t* tail;
} list_t;

#define      list_init() {0, 0}

typedef struct {
    uint8_t   Type;
    uint8_t   Request;
    uint8_t   ValueLo;
    uint8_t   ValueHi;
    uint16_t  Index;
    uint16_t  Length;
} __attribute__((packed)) Request_t;

typedef enum { Endpoint_Out, Endpoint_In, Endpoint_Bidirectional } EndpointDirection_t;
typedef enum { Endpoint_Control, Endpoint_Isochronous, Endpoint_Bulk, Endpoint_Interrupt } EndpointType_t;
typedef struct {
    EndpointDirection_t Direction;
    EndpointType_t      Type;
    uint8_t             Address;
    uint16_t            MPS;
    bool                Toggle;
    uint8_t             Interval;
} Endpoint_t;

typedef enum { USB_FULLSPEED = 1, USB_LOWSPEED = 2, USB_HIGHSPEED = 3, USB_SUPERSPEED = 4 } Speed_t;
typedef struct {
//    port_t*  Port;
    void*    Port;
    void*    Hub; // Hubs are not an interface of the device, but the device itself
    
    list_t   Endpoints;
    list_t   Interfaces;
    
    Speed_t  Speed;
    uint16_t Vendor;
    uint16_t Product;
    uint16_t ReleaseNumber;
    uint16_t UsbSpec;
    uint8_t  UsbClass;
    uint8_t  UsbSubclass;
    uint8_t  UsbProtocol;
    uint8_t  ManufacturerStringID;
    uint8_t  ProductStringID;
    uint8_t  SerialNumberStringID;
    uint8_t  NumConfigurations;
    uint8_t  Num;
    char     ProductName[16];
    char     SerialNumber[13];
    void*    TotalConfigDescriptor;
} Device_t;

#endif /* USBCommon_hpp */
