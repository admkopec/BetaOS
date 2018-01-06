//
//  USBCommon.cpp
//  Kernel
//
//  Created by Adam Kopeć on 11/24/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "USBCommon.hpp"
#include "XHCIController.hpp"
#include "EHCIController.hpp"

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a < _b ? _a : _b; })

// Descriptor Types
#define DEVICE                      1
#define CONFIGURATION               2
#define STRING                      3
#define INTERFACE                   4
#define ENDPOINT                    5
#define DEVICE_QUALIFIER            6
#define OTHER_SPEED_CONFIGURATION   7
#define INTERFACE_POWER1            8

// Standard Feature Selectors
#define DEVICE_REMOTE_WAKEUP        1
#define ENDPOINT_HALT               0
#define TEST_MODE                   2

void
USBDevice::CreateDevice(Port_t* Port, Speed_t Speed) {
    this->Port  = Port;
    this->Speed = Speed;
    Endpoints = new Endpoint_t();
    Endpoints->Address = 0;
    switch (Speed) {
        case USB_LOWSPEED: case USB_FULLSPEED:
            Endpoints->MPS = 8;
            break;
        case USB_HIGHSPEED:
            Endpoints->MPS = 64;
            break;
        case USB_SUPERSPEED:
            Endpoints->MPS = 512;
            break;
        default:
            break;
    }
    Endpoints->Direction = Endpoint_Bidirectional;
    Endpoints->Type = Endpoint_Control;
    Endpoints->Toggle = false;
    Endpoints->Interval = 0;
    memset(SerialNumber, 0, 13);
}

void
USBDevice::SetupDevice(uint8_t PortNumber) {
    Number = 0;
    Hub    = NULL;
    TotalConfigDescriptor = NULL;
    OSReturn success;
    bool fullSpeedNegotiation = Speed == USB_FULLSPEED;
    success = GetDeviceDescriptor(fullSpeedNegotiation? 8 : 18, true);
    if (success != kOSReturnSuccess) {
        success = GetDeviceDescriptor(fullSpeedNegotiation? 8 : 18, true);
    } if (success != kOSReturnSuccess) {
        printf("USBDevice: Failed while getting Device Descriptor!\n");
        return;
    }
    Number = SetDeviceAddress(PortNumber);
    if (fullSpeedNegotiation) {
        GetDeviceDescriptor(18, false);
    }
    success = GetConfigDescriptor();
    if (success != kOSReturnSuccess) {
        printf("USBDevice: Config Descriptor could not be read!\n");
        return;
    }
//    SetConfiguration(1);
    if (USBClass == 0x09) {
        #ifdef DEBUG
        printf("USBDevice: Found Hub at port %u\n", PortNumber);
        #endif
//        SetupHub();
    } else {
        bool foundSomething = false;
        for (dlelement_t* e = Interfaces.head; e; e = e->next) {
            Interface* interface = (Interface*)e->data;
            if (interface->Descriptor.InterfaceClass == 0x08 && (interface->Descriptor.InterfaceSubclass == 0x06 || interface->Descriptor.InterfaceSubclass == 0x04)) {
                foundSomething = true;
                #ifdef DEBUG
                printf("USBDevice: Found Mass Storage Device!\n");
                #endif
            } else if (interface->Descriptor.InterfaceClass == 0x03) {
                foundSomething = true;
                #ifdef DEBUG
                printf("USBDevice: Found HID Device!\n");
                #endif
            } else if (interface->Descriptor.InterfaceClass == 0x0E && interface->Descriptor.InterfaceSubclass == 0x01) {
                foundSomething = true;
                #ifdef DEBUG
                printf("USBDevice: Found Video Device!\n");
                #endif
            }
        }
        if (!foundSomething) {
            printf("USBDevice: Unsupported Device Type!\n");
        }
    }
    free(TotalConfigDescriptor);
    TotalConfigDescriptor = NULL;
}

OSReturn
USBDevice::GetDeviceDescriptor(uint8_t Length, bool first) {
    DeviceDescriptor descriptor;
    OSReturn success = ControlIn(&descriptor, 0x80, GET_DESCRIPTOR, 1, 0, 0, Length);
    if (success == kOSReturnSuccess) {
        AnalyzeDeviceDescriptor(&descriptor);
    }
    return success;
}

OSReturn
USBDevice::GetConfigDescriptor() {
    return kOSReturnError;
}

OSReturn
USBDevice::ControlIn(void *buffer, uint8_t Type, uint8_t Request, uint8_t HiValue, uint8_t LoValue, uint16_t Index, uint16_t Length) {
    Transfer* transfer = new Transfer(this, USB_CONTROL, Endpoints);
    transfer->SetupTransaction(Type, Request, HiValue, LoValue, Index, Length);
//    transfer->InTransaction(false, buffer, Length);
//    transfer->OutTransaction(true, NULL, 0);
//    transfer->ScheduleTransfer();
//    transfer->WaitForTransfer();
    OSReturn ret = transfer->Success;
    delete transfer;
    return ret;
}

uint8_t
USBDevice::SetDeviceAddress(uint8_t Number) {
    uint8_t newAddress = Number;
    Transfer* transfer = new Transfer(this, USB_CONTROL, Endpoints);
    transfer->SetupTransaction(0x00, SET_ADDRESS, 0, newAddress, 0, 0);
    transfer->InTransaction(true, 0, 0);
    transfer->ScheduleTransfer();
    transfer->WaitForTransfer();
    delete transfer;
    return newAddress;
}

void
USBDevice::AnalyzeDeviceDescriptor(const struct DeviceDescriptor *descriptor) {
    UsbSpec     = descriptor->BCDUSB;
    USBClass    = descriptor->DeviceClass;
    USBSubclass = descriptor->DeviceSubclass;
    USBProtocol = descriptor->DeviceProtocol;
    
    bool changedMPS = Endpoints->MPS != descriptor->MaxPacketSize;
    if (changedMPS) {
        Endpoints->MPS = descriptor->MaxPacketSize;
        //UpdateEndpointInformation
    }
    if (descriptor->Length > 8) {
        Vendor                  = descriptor->IDVendor;
        Product                 = descriptor->IDProduct;
        ReleaseNumber           = descriptor->BCDDevice;
        ManufacturerStringID    = descriptor->Manufacturer;
        ProductStringID         = descriptor->Product;
        SerialNumberStringID    = descriptor->SerialNumber;
        NumberOfConfigurations  = descriptor->NumConfigurations;
    }
}

void
USBHostController::ConstructRootPorts() {
    HCPort* rootPorts = new HCPort[RootHubNumberOfPorts]();
    Ports = new HCPort*[RootHubNumberOfPorts]();
    PortsSize = RootHubNumberOfPorts;
    for (uint8_t i = 0; i < RootHubNumberOfPorts; i++) {
        Ports[i] = rootPorts+i;
        ConstructPort(i, &HCType);
    }
}

void
USBHostController::ConstructPort(uint8_t Number, const PortType* Type) {
    HCPort* port = GetPort(Number);
    
    port->Connected = false;
    port->HostController = this;
    port->Device = NULL;
    port->Port.Data = port;
    port->Port.Type = Type;
    
    if (*Type == USB_XHCI) {
        snprintf(port->Port.Name, 15, "XHCI-Port %u", Number+1);
    } else if (*Type == USB_EHCI) {
        snprintf(port->Port.Name, 15, "EHCI-Port %u", Number+1);
    } else if (*Type == USB_OHCI) {
        snprintf(port->Port.Name, 15, "OHCI-Port %u", Number+1);
    } else if (*Type == USB_UHCI) {
        snprintf(port->Port.Name, 15, "UHCI-Port %u", Number+1);
    } else {
        printf("USBHostController: Unknown Port Type!\n");
    }
}

void
USBHostController::SetupUSBDevice(uint8_t PortNumber, Speed_t Speed) {
    HCPort* port = GetPort(PortNumber);
    port->Connected = true;
    port->Device    = new USBDevice();
    port->Device->CreateDevice(&port->Port, Speed);
    
    if (*port->Port.Type == USB_XHCI) {
        port->Data = (void*)(uintptr_t)(PortNumber+1);
        bool atHub = PortNumber >= RootPortCount;
        if (atHub) {
            port->Device->SetupDevice(PortNumber+1);
        } else {
            uint8_t SlotNumber = ((XHCI*)this)->PortSlotLinks[PortNumber].SlotNumber;
            port->Device->SetupDevice(SlotNumber);
        }
    } else {
        port->Device->SetupDevice(PortNumber+1);
    }
}

USBHostController::HCPort*
USBHostController::GetPort(uint8_t Number) {
    if (Number < PortsSize) {
        return Ports[Number];
    }
    return NULL;
}

uint8_t
USBHostController::AddPort(USBDevice* Device) {
    USBHostController* HostController = ((HCPort*)Device->Port->Data)->HostController;
    HCPort* Port = new HCPort();
    uint8_t num = AquirePort(Port);
    HostController->ConstructPort(num, Device->Port->Type);
    return num;
}

uint8_t
USBHostController::AquirePort(HCPort* Data) {
    size_t i = RootPortCount;
    for (; i < PortsSize; i++) {
        if (Ports[i] == 0) {
            Ports[i] = Data;
            return i;
        }
    }
    Ports = new HCPort*[PortsSize+1](); // Should be append()
    PortsSize++;
    Ports[i] = Data;
    return i;
}

uint8_t
USBHostController::SetupTransaction(Transfer *transfer, Transaction *transaction, bool toggle, uint8_t type, uint8_t Request, uint8_t HiVal, uint8_t LoVal, uint16_t Index, uint16_t Length) {
    printf("USBHostController: Function shouldn't be called!\n");
    return LoVal;
}

void
USBHostController::InTransaction(Transfer *transfer, Transaction *transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingIn) {
    printf("USBHostController: Function shouldn't be called!\n");
}

void
USBHostController::OutTransaction(Transfer *transfer, Transaction *transaction, bool toggle, void *buffer, size_t Length, uint16_t remainingOut) {
    printf("USBHostController: Function shouldn't be called!\n");
}

void
USBHostController::ScheduleTransfer(Transfer *transfer) {
    printf("USBHostController: Function shouldn't be called!\n");
}

void
USBHostController::WaitForTransfer(Transfer *transfer) {
    printf("USBHostController: Function shouldn't be called!\n");
}

Transfer::Transfer(USBDevice* device, TransferType type, Endpoint_t* endpoint) {
    Device     = device;
    Endpoint   = endpoint;
    Type       = type;
    PacketSize = endpoint->MPS;
    handler    = NULL;
    Success    = kOSReturnFailed;
    
    switch (*Device->Port->Type) {
        case USB_XHCI:
            Data = NULL;
            break;
        case USB_EHCI:
            if (Type != USB_ISOCHRONOUS) {
                Data = malloc(sizeof(EhciQH));
            } else {
                printf("Transfer: Isochronous is not supported yet!\n");
            }
            break;
        case USB_OHCI:
            printf("Transfer: USB OHCI\n");
            break;
        case USB_UHCI:
            printf("Transfer: USB UHCI\n");
            break;
        default:
            printf("Transfer: Unknown port type!\n");
            break;
    }
}

uint8_t
Transfer::SetupTransaction(uint8_t type, uint8_t Request, uint8_t HiVal, uint8_t LoVal, uint16_t Index, uint16_t Length) {
    Transaction* transaction = new Transaction();
    transaction->Type = USB_TT_SETUP;
    uint8_t retVal = LoVal;
    XHCI* HC = (XHCI*)((USBHostController::HCPort*)Device->Port->Data);
//    USBHostController* HC = (USBHostController*)((USBHostController::HCPort*)Device->Port->Data);
    printf("Transfer: Got Here, HC = %p\n", HC);
//    retVal = HC->SetupTransaction(this, transaction, false, type, Request, HiVal, LoVal, Index, Length);
    printf("USBDevice: Got Here1\n");
    Transactions[LastTransaction] = transaction;
    LastTransaction++;
    Endpoint->Toggle = true;
    return retVal;
}

void
Transfer::InTransaction(bool ControlHandshake, void *buffer, size_t Length) {
    size_t clampedLength;
    uint16_t remainingTransactions;
    if (Type == USB_ISOCHRONOUS) {
        clampedLength = Length;
        Length -= clampedLength;
        remainingTransactions = 0;
    } else {
        clampedLength = min(PacketSize, Length);
        Length -= clampedLength;
        remainingTransactions = Length / PacketSize;
        if (Length % PacketSize != 0) {
            remainingTransactions++;
        }
    }
    Transaction* transaction = new Transaction();
    transaction->Type = USB_TT_IN;
    if (ControlHandshake) {
        Endpoint->Toggle = true;
    }
    USBHostController* HC = (USBHostController*)((USBHostController::HCPort*)Device->Port->Data);
    HC->InTransaction(this, transaction, Endpoint->Toggle, buffer, clampedLength, remainingTransactions);
    Transactions[LastTransaction] = transaction;
    LastTransaction++;
    Endpoint->Toggle = !Endpoint->Toggle;
    if (remainingTransactions > 0) {
        InTransaction(Endpoint->Toggle, (void*)(((uintptr_t)buffer) + clampedLength), Length);
    }
}

void
Transfer::OutTransaction(bool ControlHandshake, void *buffer, size_t Length) {
    size_t clampedLength = min(PacketSize, Length);
    Length -= clampedLength;
    uint16_t remainingTransactions = Length / PacketSize;
    if (Length % PacketSize != 0) {
        remainingTransactions++;
    }
    Transaction* transaction = new Transaction();
    transaction->Type = USB_TT_OUT;
    if (ControlHandshake) {
        Endpoint->Toggle = true;
    }
    USBHostController* HC = (USBHostController*)((USBHostController::HCPort*)Device->Port->Data);
    HC->OutTransaction(this, transaction, Endpoint->Toggle, buffer, clampedLength, remainingTransactions);
    Transactions[LastTransaction] = transaction;
    LastTransaction++;
    Endpoint->Toggle = !Endpoint->Toggle;
    if (remainingTransactions > 0) {
        OutTransaction(Endpoint->Toggle, (void*)(((uintptr_t)buffer) + clampedLength), Length);
    }
}

void
Transfer::ScheduleTransfer() {
    USBHostController* HC = (USBHostController*)((USBHostController::HCPort*)Device->Port->Data);
    HC->ScheduleTransfer(this);
}

void
Transfer::WaitForTransfer() {
    USBHostController* HC = (USBHostController*)((USBHostController::HCPort*)Device->Port->Data);
    HC->WaitForTransfer(this);
}

Transfer::~Transfer() {
    switch (*Device->Port->Type) {
        case USB_XHCI:
            //
            break;
        case USB_EHCI:
            //
            break;
        case USB_OHCI:
            //
            break;
        case USB_UHCI:
            //
            break;
        default:
            printf("Transfer: Unknown port type!\n");
            break;
    }
}
