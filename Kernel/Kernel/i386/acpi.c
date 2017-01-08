//
//  acpi.c
//  Kernel
//
//  Created by Adam Kopeć on 12/13/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "acpi.h"
#include "misc_protos.h"
#include "pio.h"
#include <sys/cdefs.h>

#define EFI_ACPI_TABLE_GUID \
{ 0xeb9d2d30, 0x2d88, 0x11d3, {0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }}
#define EFI_ACPI_20_TABLE_GUID \
{ 0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 }}
#define EFI_ACPI_DESCRIPTION_GUID \
{ 0x3c699197, 0x93c, 0x4c69, {0xb0, 0x6b, 0x12, 0x8a, 0xe3, 0x48, 0x1d, 0xc9 }}


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

int64_t CompareGuid(EFI_GUID *GUID1, EFI_GUID *GUID2) {
    if (GUID1->Data1 == GUID2->Data1) {
        if (GUID1->Data2 == GUID2->Data2) {
            if (GUID1->Data3 == GUID2->Data3) {
                //return 1;
                for (int i = 0; i < 8; i++) {
                    if (GUID1->Data4[i] == GUID2->Data4[i]) { } else {
                        return -1;
                    }
                }
                return 0;
            }
        }
    }
    return -1;
    /*int32_t *g1, *g2, r;
    g1 = (int32_t *) GUID1;
    g2 = (int32_t *) GUID2;
    
    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];
    
    return r;*/
}

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

dword *SMI_CMD_;
byte ACPI_ENABLE_;
byte ACPI_DISABLE_;
dword *PM1a_CNT_;
dword *PM1b_CNT_;
word SLP_TYPa_;
word SLP_TYPb_;
word SLP_EN_;
word SCI_EN_;
byte PM1_CNT_LEN_;

FADT_t * Fadt;

static int
ParseRSDP(ACPI_2_Description *RSDP) {
    ACPISDTHeader_t *XSDT, *Entry;
    uint32_t  EntryCount;
    uint64_t* EntryPtr;
    kprintf("Found RSDP, Version: %d OEM ID: %s\n", RSDP->Revision, RSDP->OemId);
    if (RSDP->Revision >= 0x02) {
        XSDT = (ACPISDTHeader_t *) (RSDP->XsdtAddress);
    } else {
        kprintf("NO XSDT table found!\n");
        return 1;
    }
    
    if (strncmp("XSDT", XSDT->Signature, 4)) {
        kprintf("Invalid XSDT!\n");
        return 1;
    }
    
    EntryCount = (XSDT->Length - sizeof(ACPISDTHeader_t)) / sizeof(uint64_t);
    kprintf("Found XSDT OEM ID: %s Entry Count: %d\n", XSDT->OEMID, EntryCount);
    
    EntryPtr = (uint64_t*) (XSDT + 1);
    for (uint32_t i = 0; i < EntryCount; i++, EntryPtr++) {
        Entry = (ACPISDTHeader_t *) ((uint64_t) (*EntryPtr));
        //kprintf("Found ACPI table: %s Version %d OEM ID: %s\n", Entry->Signature, Entry->Revision, Entry->OEMID);
        if (!strncmp("FACP", Entry->Signature, 4)) {
            Fadt = (FADT_t *)Entry;
            if (!strncmp("DSDT", ((ACPISDTHeader_t*)(Fadt->Dsdt))->Signature, 4)) {
                kprintf("Found DSDT!\n");
                // search the \_S5 package in the DSDT
                char *S5Addr = (char *) Fadt->Dsdt +36; // skip header
                int dsdtLength = ((ACPISDTHeader_t*)Fadt->Dsdt)->Length;
                while (dsdtLength-- >= 0) {
                    if (strncmp(S5Addr, "_S5_", 4) == 0) {
                        break;
                    }
                    if (memcmp(S5Addr, "_S5_", 4) == 0)
                        break;
                    S5Addr++;
                }
                // check if \_S5 was found
                if (dsdtLength > 0) {
                    // check for valid AML structure
                    if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 ) {
                        S5Addr += 5;
                        S5Addr += ((*S5Addr &0xC0)>>6) +2;   // calculate PkgLength size
                        
                        if (*S5Addr == 0x0A)
                            S5Addr++;   // skip byteprefix
                        SLP_TYPa_ = *(S5Addr)<<10;
                        S5Addr++;
                        
                        if (*S5Addr == 0x0A)
                            S5Addr++;   // skip byteprefix
                        SLP_TYPb_ = *(S5Addr)<<10;
                        
                        SMI_CMD_ = &Fadt->SMI_CommandPort;
                        
                        ACPI_ENABLE_  = Fadt->AcpiEnable;
                        ACPI_DISABLE_ = Fadt->AcpiDisable;
                        
                        PM1a_CNT_ = &Fadt->PM1aControlBlock;
                        PM1b_CNT_ = &Fadt->PM1bControlBlock;
                        
                        PM1_CNT_LEN_ = Fadt->PM1ControlLength;
                        
                        SLP_EN_ = 1<<13;
                        SCI_EN_ = 1;
                        
                        return 0;
                    } else {
                        kprintf("\\_S5 parse error.\n");
                    }
                } else {
                    kprintf("\\_S5 not present.\n");
                }
            }
        }
    }
    
    return 0;
}

int acpiEnable_(void) {
    // check if acpi is enabled
    if ( (inw((word) PM1a_CNT_) &SCI_EN_) == 0 ) {
        // check if acpi can be enabled
        if (SMI_CMD_ != 0 && ACPI_ENABLE_ != 0) {
            outb((word) SMI_CMD_, ACPI_ENABLE_); // send acpi enable command
            // give 3 seconds time to enable acpi
            int i;
            for (i=0; i<300; i++ ) {
                if ( (inw((word) PM1a_CNT_) &SCI_EN_) == 1 )
                    break;
                for (int j = 0; j < 1000; j++) {
                    kprintf("");
                }
            }
            if (PM1b_CNT_ != 0)
                for (; i<300; i++ ) {
                    if ( (inw((word) PM1b_CNT_) &SCI_EN_) == 1 )
                        break;
                    for (int j = 0; j < 1000; j++) {
                        kprintf("");
                    }
                }
            if (i<300) {
                kprintf("Enabled ACPI.\n");
                return 0;
            } else {
                kprintf("Couldn't enable ACPI.\n");
                return -1;
            }
        } else {
            kprintf("No known way to enable ACPI.\n");
            return -1;
        }
    } else {
        kprintf("ACPI was already enabled.\n");
        return 0;
    }
}

void printGuid(EFI_GUID g1) {
    kprintf("{0x%lx, 0x%x, 0x%x, {0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x}}\n", g1.Data1, g1.Data2, g1.Data3, g1.Data4[0], g1.Data4[1], g1.Data4[2], g1.Data4[3], g1.Data4[4], g1.Data4[5], g1.Data4[6], g1.Data4[7]);
}

void acpi() {
    if (Platform_state.bootArgs->efiMode != kBootArgsEfiMode64) {
        panic("EFI MODE 32-bit!\n");
    }
    EFI_SYSTEM_TABLE_64 *systab = (EFI_SYSTEM_TABLE_64 *)(Platform_state.bootArgs->efiSystemTable);
    EFI_CONFIGURATION_TABLE_64 *configtab = systab->ConfigurationTable;
    ACPI_2_Description *RSDP = (void *)0;
    EFI_GUID ACPIGUID     = EFI_ACPI_20_TABLE_GUID;
    EFI_GUID ACPIGUID_old = EFI_ACPI_TABLE_GUID;
    for (uint64_t i = 0; i <= systab->NumberOfTableEntries; i++) {
        if (!(CompareGuid(&(systab->ConfigurationTable[i].VendorGuid), &ACPIGUID)) || !(CompareGuid(&(systab->ConfigurationTable[i].VendorGuid), &ACPIGUID_old))) {
            if (!strncmp("RSD PTR ", ((ACPI_2_Description *)(configtab->VendorTable))->Signature, 8)) {
                RSDP = (ACPI_2_Description *)(configtab->VendorTable);
                ParseRSDP(RSDP);
                if (SCI_EN_ == 0)
                    return;
                
                acpiEnable_();
                return;
           }
        }
        configtab++;
    }
    
    uint64_t Address;
    uint64_t Index;
    
    for (Address = 0xe0000; Address < 0xFFFFF; Address += 0x10) {
        if (*(uint64_t *)(Address) == 0x54445352 || !strncmp((const char*)((uint64_t *)(Address)), "RSD PTR ", 8) /*|| !strcmp((const char*)(*(uint64_t *)(Address)), "RSD PTR ") || */) {
            kprintf("Found RSDP using Legacy Method!\n");
            RSDP = (ACPI_2_Description *)Address;
        }
    }
    
    if (!strcmp(((ACPI_2_Description *)0xFE300)->Signature, "RSD PTR ")) {
        kprintf("Found RSDP using fixed address!\n");
        //RSDP = (ACPI_2_Description *)0xFE300;
    }
    
    Address = (*(uint16_t *)(uint64_t)(0x40E)) << 4;
    for (Index = 0; Index < 0x400; Index += 16) {
        if (*(uint64_t *)(Address + Index) == 0x54445352 || !strncmp((const char *)((uint64_t*)(Address + Index)), "RSD PTR ", 8)) {
            kprintf("Found RSDP using EBDA Method!\n");
            RSDP = (ACPI_2_Description *)Address;
        }
    }
    
    if (RSDP == (void *)0) {
        kprintf("Error: Could not find RSDP\n");
        return;
    }
    
    ParseRSDP(RSDP);
    if (SCI_EN_ == 0) {
        kprintf("SCI_EN_ == 0");
        return;
    }
    
    acpiEnable_();
}

void acpipoweroff(void) {
    outw((unsigned int)(*(PM1a_CNT_)), SLP_TYPa_ | SLP_EN_);
    if (PM1b_CNT_ != 0) {
        outw((unsigned int)(*(PM1b_CNT_)), SLP_TYPb_ | SLP_EN_);
    }
    kprintf("ACPI poweroff Failed!\n");
}

void acpireboot(void) {
    if (Fadt->h.Revision < 2) {
        kprintf("FADT isn't 2.0 or newer!\n");
        return;
    } else {
        if (CHECK_BIT(Fadt->Flags, 10)) {
            kprintf("Bit 10 is not set!\n");
            return;
        }
    }
    kprintf("%u\n%d\n", Fadt->ResetReg.Address, Fadt->ResetReg.Address);
    kprintf("%d\n", Fadt->ResetValue);
    outb(Fadt->ResetReg.Address, Fadt->ResetValue);
    kprintf("ACPI reboot Failed!\n");
}
