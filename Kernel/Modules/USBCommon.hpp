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
#include <OSObject.hpp>

// Standard Request Codes
#define GET_STATUS          0
#define CLEAR_FEATURE       1
#define SET_FEATURE         3
#define SET_ADDRESS         5
#define GET_DESCRIPTOR      6
#define SET_DESCRIPTOR      7
#define GET_CONFIGURATION   8
#define SET_CONFIGURATION   9
#define GET_INTERFACE      10
#define SET_INTERFACE      11
#define SYNCH_FRAME        12

#define kMaxRootPorts   30
#define kMaxRootSlots   16
#define kUSBMaxDevices  128

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

struct DeviceDescriptor {
    uint8_t  Length;            // 18
    uint8_t  DescriptorType;    // 1
    uint16_t BCDUSB;            // e.g. 0x0210 means 2.10
    uint8_t  DeviceClass;
    uint8_t  DeviceSubclass;
    uint8_t  DeviceProtocol;
    uint8_t  MaxPacketSize;     // MPS0, must be 8,16,32,64
    uint16_t IDVendor;
    uint16_t IDProduct;
    uint16_t BCDDevice;         // release of the device
    uint8_t  Manufacturer;
    uint8_t  Product;
    uint8_t  SerialNumber;
    uint8_t  NumConfigurations; // number of possible configurations
};

struct ConfigurationDescriptor {
    uint8_t  Length;            // 9
    uint8_t  DescriptorType;    // 2
    uint16_t TotalLength;
    uint8_t  NumInterfaces;
    uint8_t  ConfigurationValue;
    uint8_t  Configuration;
    uint8_t  Attributes;
    uint8_t  MaxPower;
};

struct InterfaceAssociationDescriptor {
    uint8_t  Length;            // 8
    uint8_t  DescriptorType;    // 11
    uint8_t  FirstInterface;
    uint8_t  InterfaceCount;
    uint8_t  FunctionClass;
    uint8_t  FunctionSubclass;
    uint8_t  FunctionProtocol;
    uint8_t  Function;
};

struct InterfaceDescriptor {
    uint8_t  Length;            // 9
    uint8_t  FescriptorType;    // 4
    uint8_t  InterfaceNumber;
    uint8_t  AlternateSetting;
    uint8_t  NumEndpoints;
    uint8_t  InterfaceClass;
    uint8_t  InterfaceSubclass;
    uint8_t  InterfaceProtocol;
    uint8_t  Interface;
};

struct EndpointDescriptor {
    uint8_t  Length;            // 7
    uint8_t  DescriptorType;    // 5
    uint8_t  EndpointAddress;
    uint8_t  Attributes;
    uint16_t MaxPacketSize;
    uint8_t  Interval;
};

struct StringDescriptor {
    uint8_t  Length;         // ?
    uint8_t  DescriptorType; // 3
    uint16_t WideString[10];    // n = 10 test-wise
};

struct StringDescriptorUnicode {
    uint8_t  Length;         // 2 + 2 * numUnicodeCharacters
    uint8_t  DescriptorType; // 3
    uint8_t  String[60];     // n = 30 test-wise (60, because we use uint8_t as type)
};

typedef enum { USB_BULK, USB_CONTROL, USB_INTERRUPT, USB_ISOCHRONOUS } TransferType;
typedef enum { USB_TT_OUT = 0, USB_TT_IN = 1, USB_TT_SETUP = 2 } TransactionType;

struct Transaction {
    void*           Data; // Contains pointer to *hci_transaction_t
    TransactionType Type;
};

typedef enum { USB_FULLSPEED = 1, USB_LOWSPEED = 2, USB_HIGHSPEED = 3, USB_SUPERSPEED = 4 } Speed_t;
typedef enum { USB_UHCI, USB_OHCI, USB_EHCI, USB_XHCI } PortType;

typedef struct {
    const PortType* Type;
    void* Data;
    char  Name[15];
} Port_t;

class USBDevice: OSObject {
    OSReturn ControlIn(void* buffer, uint8_t Type, uint8_t Request, uint8_t HiValue, uint8_t LoValue, uint16_t Index, uint16_t Length);
    void AnalyzeDeviceDescriptor(const struct DeviceDescriptor* descriptor);
    uint8_t SetDeviceAddress(uint8_t Number);
public:
    Port_t*  Port;
    void*    Hub; // Hubs are not an interface of the device, but the device itself
    
//    list_t   Endpoints;
    Endpoint_t* Endpoints;
    list_t   Interfaces;
    
    Speed_t  Speed;
    uint16_t Vendor;
    uint16_t Product;
    uint16_t ReleaseNumber;
    uint16_t UsbSpec;
    uint8_t  USBClass;
    uint8_t  USBSubclass;
    uint8_t  USBProtocol;
    uint8_t  ManufacturerStringID;
    uint8_t  ProductStringID;
    uint8_t  SerialNumberStringID;
    uint8_t  NumberOfConfigurations;
    uint8_t  Number;
    char     ProductName[16];
    char     SerialNumber[13];
    void*    TotalConfigDescriptor;
    
    OSReturn GetDeviceDescriptor(uint8_t length, bool first);
    OSReturn GetConfigDescriptor(void);
    void CreateDevice(Port_t* port, Speed_t speed);
    void SetupDevice(uint8_t PortNumber);
};

typedef struct {
    void*                          Data;
    USBDevice*                     Device;
    struct InterfaceDescriptor     Descriptor;
} Interface;

class Transfer: OSObject {
public:
    USBDevice*     Device;
    Endpoint_t*    Endpoint;
    TransferType   Type;
    uint32_t       PacketSize;
//    list_t         Transactions;
    Transaction*   Transactions[31];
    int            LastTransaction = 0;
    void           (*handler)(Transfer*, void*); // Handler to be called on successful completion of transfer
    void*          HandlerData;
    void*          Data;
    uint8_t        Frequency;
    OSReturn       Success;
    
    Transfer(USBDevice* device, TransferType type, Endpoint_t* endpoint);
    ~Transfer();
    
    uint8_t SetupTransaction(uint8_t type, uint8_t Request, uint8_t HiVal, uint8_t LoVal, uint16_t Index, uint16_t Length);
    void InTransaction(bool ControlHandshake, void* buffer, size_t Length);
    void OutTransaction(bool ControlHandshake, void* buffer, size_t Length);
    void ScheduleTransfer(void);
    void WaitForTransfer(void);
};

class USBHostController: OSObject {
public:
    typedef struct {
        Port_t                Port;
        USBHostController*    HostController;
        USBDevice*            Device;
        void*                 Data;
        bool                  Connected;
    } HCPort;
protected:
    PortType HCType;
    int      RootHubNumberOfPorts;
    HCPort** Ports;
    size_t   PortsSize;
    uint8_t  RootPortCount;
    
    void ConstructRootPorts(void);
    void ConstructPort(uint8_t Number, const PortType* Type);
    HCPort* GetPort(uint8_t Number);
    uint8_t AddPort(USBDevice* Device);
    uint8_t AquirePort(HCPort* Data);
public:
    void SetupUSBDevice(uint8_t PortNumber, Speed_t Speed);
    virtual uint8_t      SetupTransaction(Transfer* transfer, Transaction* transaction, bool toggle, uint8_t type, uint8_t Request, uint8_t HiVal, uint8_t LoVal, uint16_t Index, uint16_t Length);
    virtual void InTransaction(Transfer* transfer, Transaction* transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingIn);
    virtual void OutTransaction(Transfer* transfer, Transaction* transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingOut);
    virtual void ScheduleTransfer(Transfer* transfer);
    virtual void WaitForTransfer(Transfer* transfer);
};

#endif /* USBCommon_hpp */
