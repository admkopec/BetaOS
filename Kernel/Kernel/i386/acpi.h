//
//  acpi.h
//  Kernel
//
//  Created by Adam Kopeć on 12/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef acpi_h
#define acpi_h

#include <stdint.h>
#include <platform/efi.h>
#include <platform/platform.h>

typedef struct {
    char     Signature[8];
    uint8_t  Checksum;
    uint8_t  OemId[6];
    uint8_t  Revision;
    uint32_t RsdtAddress;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t  ExtendedChecksum;
    uint8_t  Reserved[3];
} ACPI_2_Description;

typedef struct {
    char anchor[4];         // '_SM_'
    unsigned char esp_checksum;
    unsigned char ep_length;
    unsigned char major_version;
    unsigned char minor_version;
    unsigned short max_structure_size;
    unsigned char eps_revision;
    unsigned char formatted_area[5];
    char dmi_anchor[5];     // '_DMI_'
    unsigned char intermediate_checksum;
    unsigned short table_length;
    unsigned int  table_address;
    unsigned short entry_count;
    unsigned char bcd_revision;
} SMBIOSHeader;

typedef struct {
    SMBIOSHeader * SMBIOS;
    uintptr_t      OriginalAddress;
    bool           foundInBios;
} SMBIOS_for_Swift;

typedef struct {
    ACPI_2_Description *RSDP;
    uintptr_t           OriginalAddress;
    bool                foundInBios;
} RSDP_for_Swift;

typedef struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} ACPISDTHeader_t;

typedef struct GenericAddressStructure_ {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} GenericAddressStructure;

typedef struct FADT {
    struct   ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
    
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
    
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
    
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
    
    uint8_t  Reserved2;
    uint32_t Flags;
    
    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
    
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
    
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
    
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
} FADT_t;

typedef struct MADT {
    struct ACPISDTHeader h;
    uint32_t LocalControllerAddresss;
    uint32_t Flags;
    uint8_t  Entry;
    uint8_t  EntryLength;
} MADT_t;

typedef struct {
    uint8_t EntryType;
    uint8_t EntryLength;
    uint8_t ID;
    uint8_t Reserved;
    uint32_t Address;
    uint32_t Interrupt;
} IOAPICEntry;

void acpi(void);
void acpipoweroff(void);
void acpireboot(void);

#endif /* acpi_h */
