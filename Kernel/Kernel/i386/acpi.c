//
//  acpi.c
//  Kernel
//
//  Created by Adam Kopeć on 12/13/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include "acpi.h"
#include "misc_protos.h"
#include "pio.h"
#include "machine_routines.h"
#include <sys/cdefs.h>

#define EFI_ACPI_TABLE_GUID \
{ 0xeb9d2d30, 0x2d88, 0x11d3, {0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }}
#define EFI_ACPI_TABLE_GUID_2 \
{ 0xeb9d2d31, 0x2d88, 0x11d3, {0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }}
#define EFI_ACPI_20_TABLE_GUID \
{ 0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 }}
#define EFI_ACPI_DESCRIPTION_GUID \
{ 0x3c699197, 0x93c, 0x4c69, {0xb0, 0x6b, 0x12, 0x8a, 0xe3, 0x48, 0x1d, 0xc9 }}

#if DEBUG
#define DBG(x...)   kprintf(x)
#else
#define DBG(x...)
#endif


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

dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
dword  PM1a;
dword  PM1b;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;

extern uint32_t IOAPICAddressPhys;

FADT_t * Fadt;
MADT_t * Madt;

static int
ParseRSDP(ACPI_2_Description *RSDP) {
    ACPISDTHeader_t *XSDT, *Entry;
    uint32_t  EntryCount;
    uint64_t* EntryPtr;
    DBG("Found RSDP, Version: %d OEM ID: %s\n", RSDP->Revision, RSDP->OemId);
    if (RSDP->Revision >= 0x02) {
        XSDT = (ACPISDTHeader_t *) (RSDP->XsdtAddress);
    } else {
        DBG("NO XSDT table found!\n");
        return 1;
    }

    if (strncmp("XSDT", XSDT->Signature, 4)) {
        DBG("Invalid XSDT!\n");
        return 1;
    }

    EntryCount = (XSDT->Length - sizeof(ACPISDTHeader_t)) / sizeof(uint64_t);
    DBG("Found XSDT OEM ID: %s Entry Count: %d\n", XSDT->OEMID, EntryCount);

    EntryPtr = (uint64_t*) (XSDT + 1);
    for (uint32_t i = 0; i < EntryCount; i++, EntryPtr++) {
        Entry = (ACPISDTHeader_t *) ((uint64_t) (*EntryPtr));
        if (!strncmp("APIC", Entry->Signature, 4)) {
            DBG("Found MADT!\n");
            Madt = (MADT_t *)Entry;
            uint8_t* e;
            uint8_t* eLength;
            
            e = &Madt->Entry;
            eLength = &Madt->EntryLength;
            
            while (e < (uint8_t*)(Madt + Madt->h.Length)) {
                if (*e == 1) {
                    IOAPICAddressPhys = *((uint32_t*)(e + 4));
                    DBG("IOAPIC Address: %X\n", IOAPICAddressPhys);
                    break;
                }
                e = e + *eLength;
                eLength = eLength + *eLength;
            }
        }
        if (!strncmp("FACP", Entry->Signature, 4)) {
            Fadt = (FADT_t *)Entry;
            if (!strncmp("DSDT", ((ACPISDTHeader_t*)(uintptr_t)(Fadt->Dsdt))->Signature, 4)) {
                DBG("Found DSDT!\n");
                // search the \_S5 package in the DSDT
                char *S5Addr = (char *)(uintptr_t)Fadt->Dsdt +36; // skip header
                int dsdtLength = ((ACPISDTHeader_t*)(uintptr_t)Fadt->Dsdt)->Length;
                while (dsdtLength-- >= 0) {
                    if (strncmp(S5Addr, "_S5_", 4) == 0) {
                        break;
                    }
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
                        SLP_TYPa = *(S5Addr)<<10;
                        S5Addr++;

                        if (*S5Addr == 0x0A)
                            S5Addr++;   // skip byteprefix
                        SLP_TYPb = *(S5Addr)<<10;

                        SMI_CMD = &Fadt->SMI_CommandPort;

                        ACPI_ENABLE  = Fadt->AcpiEnable;
                        ACPI_DISABLE = Fadt->AcpiDisable;

                        PM1a_CNT = &Fadt->PM1aControlBlock;
                        PM1b_CNT = &Fadt->PM1bControlBlock;

                        PM1a = *PM1a_CNT;
                        PM1b = *PM1b_CNT;

                        PM1_CNT_LEN = Fadt->PM1ControlLength;

                        SLP_EN = 1<<13;
                        SCI_EN = 1;

                        //return 0;
                    } else {
                        DBG("\\_S5 parse error.\n");
                    }
                } else {
                    DBG("\\_S5 not present.\n");
                }
            } /*else if (!strncmp("APIC", Entry->Signature, 4)) {
                Madt = (MADT_t *)Entry;
                Entry_Madt* eMadt = (Entry_Madt *)&Madt->Entry;
                for (bool break_ = false; break_ != true;) {
                    if (eMadt->EntryType == 1) {
                        IOAPICAddress = *(&eMadt->EntryType + sizeof(uint8_t)*4);
                        kprintf("Found IOAPIC: Address: %d\n", IOAPICAddress);
                        break_ = true;
                        break;
                    }
                    eMadt = (Entry_Madt *)eMadt + eMadt->EntryLength;
                }
            }*/
        }
    }

    return 0;
}

int acpiEnable_(void) {
    // check if acpi is enabled
    if ( (inw((word) PM1a_CNT) &SCI_EN) == 0 ) {
        // check if acpi can be enabled
        if (SMI_CMD != 0 && ACPI_ENABLE != 0) {
            outb((word) SMI_CMD, ACPI_ENABLE); // send acpi enable command
            // give 3 seconds time to enable acpi
            int i;
            for (i=0; i<300; i++ ) {
                if ( (inw((word) PM1a_CNT) &SCI_EN) == 1 )
                    break;
                for (int j = 0; j < 1000; j++) {
                    kprintf("");
                }
            }
            if (PM1b_CNT != 0)
                for (; i<300; i++ ) {
                    if ( (inw((word) PM1b_CNT) &SCI_EN) == 1 )
                        break;
                    for (int j = 0; j < 1000; j++) {
                        kprintf("");
                    }
                }
            if (i<300) {
                DBG("Enabled ACPI.\n");
                return 0;
            } else {
                DBG("Couldn't enable ACPI.\n");
                return -1;
            }
        } else {
            DBG("No known way to enable ACPI.\n");
            return -1;
        }
    } else {
        DBG("ACPI was already enabled.\n");
        return 0;
    }
}

void printGuid(EFI_GUID g1) {
    DBG("0x%lX-0x%X-0x%X-0x%X%X-0x%X%X%X%X%X%X\n", g1.Data1, g1.Data2, g1.Data3, g1.Data4[0], g1.Data4[1], g1.Data4[2], g1.Data4[3], g1.Data4[4], g1.Data4[5], g1.Data4[6], g1.Data4[7]);
}

void acpi() {
    uint64_t Address;
    uint64_t Index;
    ACPI_2_Description *RSDP = (void *)0;
    
    //printGuid(((EFI_SYSTEM_TABLE_64 *)((uint64_t)(Platform_state.bootArgs->efiSystemTable)))->ConfigurationTable[4].VendorGuid);  // tab[4] for M4600 is APCI 2.0

    for (Address = 0xe0000; Address < 0xFFFFF; Address += 0x10) {
        if (*(uint64_t *)(ml_static_ptovirt(Address)) == 0x54445352 || !strncmp((const char*)((uint64_t *)(ml_static_ptovirt(Address))), "RSD PTR ", 8)) {
            DBG("Found RSDP using Legacy Method!\n");
            RSDP = (ACPI_2_Description *)ml_static_ptovirt(Address);
            goto parse;
        }
    }

    Address = (*(uint16_t *)(uint64_t)(0x40E)) << 4;
    for (Index = 0; Index < 0x400; Index += 16) {
        if (*(uint64_t *)(ml_static_ptovirt(Address + Index)) == 0x54445352 || !strncmp((const char *)((uint64_t*)(ml_static_ptovirt(Address + Index))), "RSD PTR ", 8)) {
            DBG("Found RSDP using EBDA Method!\n");
            RSDP = (ACPI_2_Description *)ml_static_ptovirt(Address);
            goto parse;
        }
    }

    if (Platform_state.bootArgs->efiMode != kBootArgsEfiMode64) {
        panic("EFI MODE 32-bit!\n");
    }
    EFI_SYSTEM_TABLE_64 *systab = (EFI_SYSTEM_TABLE_64 *)((uint64_t)(Platform_state.bootArgs->efiSystemTable));
    EFI_CONFIGURATION_TABLE_64 *configtab = systab->ConfigurationTable;
    EFI_GUID ACPIGUID      = EFI_ACPI_20_TABLE_GUID;
    EFI_GUID ACPIGUID_old2 = EFI_ACPI_TABLE_GUID_2;
    EFI_GUID ACPIGUID_old  = EFI_ACPI_TABLE_GUID;
    for (uint64_t i = 0; i <= systab->NumberOfTableEntries; i++) {
        if (!(CompareGuid(&(systab->ConfigurationTable[i].VendorGuid), &ACPIGUID)) || !(CompareGuid(&(systab->ConfigurationTable[i].VendorGuid), &ACPIGUID_old)) || !(CompareGuid(&(systab->ConfigurationTable[i].VendorGuid), &ACPIGUID_old2))) {
            if (!strncmp("RSD PTR ", ((ACPI_2_Description *)(configtab->VendorTable))->Signature, 8)) {
                RSDP = (ACPI_2_Description *)(configtab->VendorTable);
                goto parse;
            }
        }
        configtab++;
    }

    if (RSDP == (void *)0) {
        DBG("Error: Could not find RSDP\n");
        return;
    }
parse:
    ParseRSDP(RSDP);
    if (SCI_EN == 0) {
        DBG("SCI_EN == 0");
        return;
    }

    acpiEnable_();
}

void acpipoweroff(void) {
    outw((unsigned int)(PM1a), SLP_TYPa | SLP_EN);
    if (PM1b_CNT != 0) {
        outw((unsigned int)(PM1b), SLP_TYPb | SLP_EN);
    }
    kprintf("ACPI poweroff Failed!\n");
}

void acpireboot(void) {
    if (Fadt->h.Revision < 2) {
        DBG("FADT isn't 2.0 or newer!\n");
        return;
    } else {
        if (CHECK_BIT(Fadt->Flags, 10)) {
            DBG("Bit 10 is not set!\n");
            return;
        }
    }
    DBG("%u\n%d\n", Fadt->ResetReg.Address, Fadt->ResetReg.Address);
    DBG("%d\n", Fadt->ResetValue);
    outb((unsigned int)Fadt->ResetReg.Address, Fadt->ResetValue);
    kprintf("ACPI reboot Failed!\n");
}
