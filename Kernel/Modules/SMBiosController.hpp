//
//  SMBiosController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 2/12/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef SMBiosController_hpp
#define SMBiosController_hpp

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)             // enable 8-bit struct packing

struct DMIEntryPoint {
    uint8_t     signature[5];
    uint8_t     checksum;
    uint16_t    tableLength;
    uint32_t    tableAddress;
    uint16_t    structureCount;
    uint8_t     bcdRevision;
};

struct SMBEntryPoint {
    uint8_t     signature[4];
    uint8_t     checksum;
    uint8_t     entryPointLength;
    uint8_t     majorVersion;
    uint8_t     minorVersion;
    uint16_t    maxStructureSize;
    uint8_t    entryPointRevision;
    uint8_t    formattedArea[5];
    struct DMIEntryPoint dmi;
};

//
// Header common to all SMBIOS structures
//

struct SMBStructHeader {
    uint8_t     type;
    uint8_t     length;
    uint16_t    handle;
};

#define SMB_STRUCT_HEADER  SMBStructHeader header;

struct SMBAnchor {
    const SMBStructHeader *	header;
    const uint8_t *			next;
    const uint8_t *			end;
};

#define SMB_ANCHOR_IS_VALID(x)	\
((x) && ((x)->header) && ((x)->next) && ((x)->end))

#define SMB_ANCHOR_RESET(x)		\
bzero(x, sizeof(struct SMBAnchor));

//
// SMBIOS structure types.
//

enum {
    kSMBTypeBIOSInformation             =  0,
    kSMBTypeSystemInformation           =  1,
    kSMBTypeBaseBoard					=  2,
    kSMBTypeSystemEnclosure             =  3,
    kSMBTypeProcessorInformation        =  4,
    kSMBTypeMemoryModule                =  6,
    kSMBTypeCacheInformation            =  7,
    kSMBTypeSystemSlot                  =  9,
    kSMBTypePhysicalMemoryArray         = 16,
    kSMBTypeMemoryDevice                = 17,
    kSMBType32BitMemoryErrorInfo        = 18,
    kSMBType64BitMemoryErrorInfo        = 33,
    
    /* Apple Specific Structures */
    kSMBTypeFirmwareVolume              = 128,
    kSMBTypeMemorySPD                   = 130,
    kSMBTypeOemProcessorType            = 131,
    kSMBTypeOemProcessorBusSpeed        = 132
};

//
// BIOS Information (Type 0)
//
struct SMBBIOSInformation {
    SMB_STRUCT_HEADER               // Type 0
    uint8_t  vendor;              // BIOS vendor name
    uint8_t  version;             // BIOS version
    uint16_t    startSegment;        // BIOS segment start
    uint8_t  releaseDate;         // BIOS release date
    uint8_t    romSize;             // (n); 64K * (n+1) bytes
    uint64_t   characteristics;     // supported BIOS functions
};

//
// System Information (Type 1)
//

struct SMBSystemInformation {
    // 2.0+ spec (8 bytes)
    SMB_STRUCT_HEADER               // Type 1
    uint8_t  manufacturer;
    uint8_t  productName;
    uint8_t  version;
    uint8_t  serialNumber;
    // 2.1+ spec (25 bytes)
    uint8_t    uuid[16];            // can be all 0 or all 1's
    uint8_t    wakeupReason;        // reason for system wakeup
};

//
// Base Board (Type 2)
//

struct SMBBaseBoard {
    SMB_STRUCT_HEADER               // Type 2
    uint8_t	manufacturer;
    uint8_t	product;
    uint8_t	version;
    uint8_t	serialNumber;
    uint8_t	assetTagNumber;
    uint8_t		featureFlags;
    uint8_t	locationInChassis;
    uint16_t		chassisHandle;
    uint8_t		boardType;
    uint8_t		numberOfContainedHandles;
    // 0 - 255 contained handles go here but we do not include
    // them in our structure. Be careful to use numberOfContainedHandles
    // times sizeof(uint16_t) when computing the actual record size,
    // if you need it.
};

// Values for boardType in Type 2 records
enum {
    kSMBBaseBoardUnknown				= 0x01,
    kSMBBaseBoardOther					= 0x02,
    kSMBBaseBoardServerBlade			= 0x03,
    kSMBBaseBoardConnectivitySwitch		= 0x04,
    kSMBBaseBoardSystemMgmtModule		= 0x05,
    kSMBBaseBoardProcessorModule		= 0x06,
    kSMBBaseBoardIOModule				= 0x07,
    kSMBBaseBoardMemoryModule			= 0x08,
    kSMBBaseBoardDaughter				= 0x09,
    kSMBBaseBoardMotherboard			= 0x0A,
    kSMBBaseBoardProcessorMemoryModule	= 0x0B,
    kSMBBaseBoardProcessorIOModule		= 0x0C,
    kSMBBaseBoardInterconnect			= 0x0D,
};


//
// System Enclosure (Type 3)
//

struct SMBSystemEnclosure {
    SMB_STRUCT_HEADER               // Type 3
    uint8_t  manufacturer;
    uint8_t    type;
    uint8_t  version;
    uint8_t  serialNumber;
    uint8_t  assetTagNumber;
    uint8_t    bootupState;
    uint8_t    powerSupplyState;
    uint8_t    thermalState;
    uint8_t    securityStatus;
    uint32_t   oemDefined;
};

//
// Processor Information (Type 4)
//

struct SMBProcessorInformation {
    // 2.0+ spec (26 bytes)
    SMB_STRUCT_HEADER               // Type 4
    uint8_t  socketDesignation;
    uint8_t    processorType;       // CPU = 3
    uint8_t    processorFamily;     // processor family enum
    uint8_t  manufacturer;
    uint64_t   processorID;         // based on CPUID
    uint8_t  processorVersion;
    uint8_t    voltage;             // bit7 cleared indicate legacy mode
    uint16_t    externalClock;       // external clock in MHz
    uint16_t    maximumClock;        // max internal clock in MHz
    uint16_t    currentClock;        // current internal clock in MHz
    uint8_t    status;
    uint8_t    processorUpgrade;    // processor upgrade enum
    // 2.1+ spec (32 bytes)
    uint16_t    L1CacheHandle;
    uint16_t    L2CacheHandle;
    uint16_t    L3CacheHandle;
    // 2.3+ spec (35 bytes)
    uint8_t  serialNumber;
    uint8_t  assetTag;
    uint8_t  partNumber;
};

#define kSMBProcessorInformationMinSize     26

//
// Memory Module Information (Type 6)
// Obsoleted since SMBIOS version 2.1
//

struct SMBMemoryModule {
    SMB_STRUCT_HEADER               // Type 6
    uint8_t  socketDesignation;
    uint8_t    bankConnections;
    uint8_t    currentSpeed;
    uint16_t    currentMemoryType;
    uint8_t    installedSize;
    uint8_t    enabledSize;
    uint8_t    errorStatus;
};

#define kSMBMemoryModuleSizeNotDeterminable 0x7D
#define kSMBMemoryModuleSizeNotEnabled      0x7E
#define kSMBMemoryModuleSizeNotInstalled    0x7F

//
// Cache Information (Type 7)
//

struct SMBCacheInformation {
    SMB_STRUCT_HEADER               // Type 7
    uint8_t  socketDesignation;
    uint16_t    cacheConfiguration;
    uint16_t    maximumCacheSize;
    uint16_t    installedSize;
    uint16_t    supportedSRAMType;
    uint16_t    currentSRAMType;
    uint8_t    cacheSpeed;
    uint8_t    errorCorrectionType;
    uint8_t    systemCacheType;
    uint8_t    associativity;
};

struct SMBSystemSlot {
    // 2.0+ spec (12 bytes)
    SMB_STRUCT_HEADER               // Type 9
    uint8_t   slotDesignation;
    uint8_t     slotType;
    uint8_t     slotDataBusWidth;
    uint8_t     currentUsage;
    uint8_t     slotLength;
    uint16_t     slotID;
    uint8_t     slotCharacteristics1;
    // 2.1+ spec (13 bytes)
    uint8_t     slotCharacteristics2;
};

//
// Physical Memory Array (Type 16)
//

struct SMBPhysicalMemoryArray {
    // 2.1+ spec (15 bytes)
    SMB_STRUCT_HEADER               // Type 16
    uint8_t    physicalLocation;    // physical location
    uint8_t    arrayUse;            // the use for the memory array
    uint8_t    errorCorrection;     // error correction/detection method
    uint32_t   maximumCapacity;     // maximum memory capacity in kilobytes
    uint16_t    errorHandle;         // handle of a previously detected error
    uint16_t    numMemoryDevices;    // number of memory slots or sockets
};

// Memory Array - Use
enum {
    kSMBMemoryArrayUseOther             = 0x01,
    kSMBMemoryArrayUseUnknown           = 0x02,
    kSMBMemoryArrayUseSystemMemory      = 0x03,
    kSMBMemoryArrayUseVideoMemory       = 0x04,
    kSMBMemoryArrayUseFlashMemory       = 0x05,
    kSMBMemoryArrayUseNonVolatileMemory = 0x06,
    kSMBMemoryArrayUseCacheMemory       = 0x07
};

// Memory Array - Error Correction Types
enum {
    kSMBMemoryArrayErrorCorrectionTypeOther         = 0x01,
    kSMBMemoryArrayErrorCorrectionTypeUnknown       = 0x02,
    kSMBMemoryArrayErrorCorrectionTypeNone          = 0x03,
    kSMBMemoryArrayErrorCorrectionTypeParity        = 0x04,
    kSMBMemoryArrayErrorCorrectionTypeSingleBitECC  = 0x05,
    kSMBMemoryArrayErrorCorrectionTypeMultiBitECC   = 0x06,
    kSMBMemoryArrayErrorCorrectionTypeCRC           = 0x07
};

//
// Memory Device (Type 17)
//

struct SMBMemoryDevice {
    // 2.1+ spec (21 bytes)
    SMB_STRUCT_HEADER               // Type 17
    uint16_t    arrayHandle;         // handle of the parent memory array
    uint16_t    errorHandle;         // handle of a previously detected error
    uint16_t    totalWidth;          // total width in bits; including ECC bits
    uint16_t    dataWidth;           // data width in bits
    uint16_t    memorySize;          // bit15 is scale, 0 = MB, 1 = KB
    uint8_t    formFactor;          // memory device form factor
    uint8_t    deviceSet;           // parent set of identical memory devices
    uint8_t  deviceLocator;       // labeled socket; e.g. "SIMM 3"
    uint8_t  bankLocator;         // labeled bank; e.g. "Bank 0" or "A"
    uint8_t    memoryType;          // type of memory
    uint16_t    memoryTypeDetail;    // additional detail on memory type
    // 2.3+ spec (27 bytes)
    uint16_t    memorySpeed;         // speed of device in MHz (0 for unknown)
    uint8_t  manufacturer;
    uint8_t  serialNumber;
    uint8_t  assetTag;
    uint8_t  partNumber;
};

//
// Firmware Volume Description (Apple Specific - Type 128)
//

enum {
    FW_REGION_RESERVED   = 0,
    FW_REGION_RECOVERY   = 1,
    FW_REGION_MAIN       = 2,
    FW_REGION_NVRAM      = 3,
    FW_REGION_CONFIG     = 4,
    FW_REGION_DIAGVAULT  = 5,
    
    NUM_FLASHMAP_ENTRIES = 8
};

struct FW_REGION_INFO
{
    uint32_t   StartAddress;
    uint32_t   EndAddress;
};

struct SMBFirmwareVolume {
    SMB_STRUCT_HEADER               // Type 128
    uint8_t           RegionCount;
    uint8_t           Reserved[3];
    uint32_t          FirmwareFeatures;
    uint32_t          FirmwareFeaturesMask;
    uint8_t           RegionType[ NUM_FLASHMAP_ENTRIES ];
    FW_REGION_INFO    FlashMap[   NUM_FLASHMAP_ENTRIES ];
};

//
// Memory SPD Data   (Apple Specific - Type 130)
//

struct SMBMemorySPD {
    SMB_STRUCT_HEADER               // Type 130
    uint16_t           Type17Handle;
    uint16_t           Offset;
    uint16_t           Size;
    uint16_t           Data[];
};

static const char *
SMBMemoryDeviceTypes[] =
{
    "RAM",          /* 00h  Undefined */
    "RAM",          /* 01h  Other */
    "RAM",          /* 02h  Unknown */
    "DRAM",         /* 03h  DRAM */
    "EDRAM",        /* 04h  EDRAM */
    "VRAM",         /* 05h  VRAM */
    "SRAM",         /* 06h  SRAM */
    "RAM",          /* 07h  RAM */
    "ROM",          /* 08h  ROM */
    "FLASH",        /* 09h  FLASH */
    "EEPROM",       /* 0Ah  EEPROM */
    "FEPROM",       /* 0Bh  FEPROM */
    "EPROM",        /* 0Ch  EPROM */
    "CDRAM",        /* 0Dh  CDRAM */
    "3DRAM",        /* 0Eh  3DRAM */
    "SDRAM",        /* 0Fh  SDRAM */
    "SGRAM",        /* 10h  SGRAM */
    "RDRAM",        /* 11h  RDRAM */
    "DDR SDRAM",    /* 12h  DDR */
    "DDR2 SDRAM",   /* 13h  DDR2 */
    "DDR2 FB-DIMM", /* 14h  DDR2 FB-DIMM */
    "RAM",			/* 15h  unused */
    "RAM",			/* 16h  unused */
    "RAM",			/* 17h  unused */
    "DDR3",			/* 18h  DDR3, chosen in [5776134] */
};

static const int
kSMBMemoryDeviceTypeCount = sizeof(SMBMemoryDeviceTypes)   /
sizeof(SMBMemoryDeviceTypes[0]);

//
// OEM Processor Type (Apple Specific - Type 131)
//

struct SMBOemProcessorType {
    SMB_STRUCT_HEADER
    uint16_t    ProcessorType;
};

//
// OEM Processor Bus Speed (Apple Specific - Type 132)
//
struct SMBOemProcessorBusSpeed {
    SMB_STRUCT_HEADER
    uint16_t    ProcessorBusSpeed;   // MT/s unit
};

#pragma pack(pop) // reset to default struct packing

class SMBiosController {
    void *      SMBiosTable;
    //uint16_t    SMBiosTableLength;
public:
    static uint16_t    SMBiosTableLength;
    int  init(void);
    void start(void);
};

#endif /* SMBiosController_hpp */
