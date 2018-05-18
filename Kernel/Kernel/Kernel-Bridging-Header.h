//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#include <kernel/command.h>
#include <platform/platform.h>

#include <i386/pio.h>
#include <i386/pal.h>
#include <i386/cpuid.h>
#include <i386/thread_status.h>
#include <i386/proc_reg.h>
#include <objc/NSObject.h>


#ifdef DEBUG
#define BUILD_TYPE "Internal"
#else
#define BUILD_TYPE ""
#endif

#define USER_CS    0x002b
#define USER32_CS  0x001b
#define USER_DS    0x0023
#define KERN_CS    0x0008
#define KERN_DS    0x0068

typedef struct {
    char           Signature[8];
    unsigned char  Checksum;
    unsigned char  OemId[6];
    unsigned char  Revision;
    unsigned int   RsdtAddress;
    unsigned int   Length;
    unsigned long long  XsdtAddress;
    unsigned char  ExtendedChecksum;
    unsigned char  Reserved[3];
} ACPI_2_Description;

typedef struct ACPISDTHeader {
    char          Signature[4];
    unsigned int  Length;
    unsigned char Revision;
    unsigned char Checksum;
    char          OEMID[6];
    char          OEMTableID[8];
    unsigned int  OEMRevision;
    char          CreatorID[4];
    unsigned int  CreatorRevision;
} ACPISDTHeader_t;

typedef struct {
    unsigned char address_space_id;
    unsigned char register_bit_width;
    unsigned char register_bit_offset;
    unsigned char access_size;
    unsigned long address;
} ACPIGenericAddressStructure;

typedef struct {
    ACPISDTHeader_t header;
    unsigned int firmware_ctrl; // 32bit address
    unsigned int dsdt;          // 32bit address
    unsigned char reserved1;
    unsigned char preferred_pm_profile;
    unsigned short sci_int;
    unsigned int smi_cmd;
    unsigned char acpi_enable;
    unsigned char acpi_disable;
    unsigned char s4bios_req;
    unsigned char pstate_cnt;
    unsigned int pm1a_evt_blk;
    unsigned int pm1b_evt_blk;
    unsigned int pm1a_cnt_blk;
    unsigned int pm1b_cnt_blk;
    unsigned int pm2_cnt_blk;
    unsigned int pm_tmr_blk;
    unsigned int gpe0_blk;
    unsigned int gpe1_blk;
    unsigned char pm1_evt_len;
    unsigned char pm1_cnt_len;
    unsigned char pm2_cnt_len;
    unsigned char pm_tmr_len;
    unsigned char gpe0_blk_len;
    unsigned char gpe1_blk_len;
    unsigned char gpe1_base;
    unsigned char cst_cnt;
    unsigned short p_lvl2_lat;
    unsigned short p_lvl3_lat;
    unsigned short flush_size;
    unsigned short flush_stride;
    unsigned char duty_offset;
    unsigned char duty_width;
    unsigned char day_alrm;
    unsigned char mon_alrm;
    unsigned char century;
    unsigned short iapc_boot_arch;
    unsigned char reserved2;
    unsigned int feature_flags;
    ACPIGenericAddressStructure reset_reg;
    unsigned char reset_value;
    unsigned short arm_boot_arch;
    unsigned char fadt_minor_version;
    unsigned long x_firmware_ctrl;
    unsigned long x_dsdt;
    ACPIGenericAddressStructure x_pm1a_evt_blk;
    ACPIGenericAddressStructure x_pm1b_evt_blk;
    ACPIGenericAddressStructure x_pm1a_cnt_blk;
    ACPIGenericAddressStructure x_pm1b_cnt_blk;
    ACPIGenericAddressStructure x_pm2_cnt_blk;
    ACPIGenericAddressStructure x_pm_tmr_blk;
    ACPIGenericAddressStructure x_gpe0_blk;
    ACPIGenericAddressStructure x_gpe1_blk;
    ACPIGenericAddressStructure sleep_control_reg;
    ACPIGenericAddressStructure sleep_status_reg;
    unsigned char hypervisor_vendor_id[8];
} ACPIFADT;

typedef struct {
    ACPISDTHeader_t header;
    unsigned int LocalInterruptControllerAddress;
    unsigned int MultipleAPICFlags;
} ACPIMADT;

typedef struct {
    ACPISDTHeader_t header;
    unsigned int timer_block_id;
    ACPIGenericAddressStructure base_address;   // Lower 32bit of address
    unsigned char hpet_number;
    unsigned short min_clock_ticks;
    unsigned char page_protection;
} ACPIHPET;

// SMBIOS Entry Point structure (EPS)
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

//#define    SATA_SIG_ATA            0x00000101    // SATA drive
//#define    SATA_SIG_ATAPI          0xEB140101    // SATAPI drive
//#define    SATA_SIG_SEMB           0xC33C0101    // Enclosure management bridge
//#define    SATA_SIG_PM             0x96690101    // Port multiplier
//#define HBA_PORT_IPM_ACTIVE     0x1
//#define HBA_PORT_DET_PRESENT    0x3
//#define HBA_NO_DEVICE           0x0
//#define AHCI_DEV_SATA           0x5A7A
//#define AHCI_DEV_SATAPI         0x5A7A314
//#define AHCI_DEV_SEMB           0x5E4DB
//#define AHCI_DEV_PM             0x504D
//#define AHCI_DEV_NULL           0x0000

typedef volatile struct tagHBA_PORT {
    uint32_t    clb;        // 0x00, command list base address, 1K-uint8_t aligned
    uint32_t    clbu;        // 0x04, command list base address upper 32 bits
    uint32_t    fb;         // 0x08, FIS base address, 256-uint8_t aligned
    uint32_t    fbu;        // 0x0C, FIS base address upper 32 bits
    uint32_t    is;         // 0x10, interrupt status
    uint32_t    ie;         // 0x14, interrupt enable
    uint32_t    cmd;        // 0x18, command and status
    uint32_t    rsv0;        // 0x1C, Reserved
    uint32_t    tfd;        // 0x20, task file data
    uint32_t    sig;        // 0x24, signature
    uint32_t    ssts;        // 0x28, SATA status (SCR0:SStatus)
    uint32_t    sctl;        // 0x2C, SATA control (SCR2:SControl)
    uint32_t    serr;        // 0x30, SATA error (SCR1:SError)
    uint32_t    sact;        // 0x34, SATA active (SCR3:SActive)
    uint32_t    ci;         // 0x38, command issue
    uint32_t    sntf;        // 0x3C, SATA notification (SCR4:SNotification)
    uint32_t    fbs;        // 0x40, FIS-based switch control
    uint32_t    rsv1[11];    // 0x44 ~ 0x6F, Reserved
    uint32_t    vendor[4];    // 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef struct {
    uint8_t        dsfis [0x1c];     // DMA Setup FIS
    uint8_t        res1  [0x04];
    uint8_t        psfis [0x14];     // PIO Setup FIS
    uint8_t        res2  [0x0c];
    uint8_t        rfis  [0x14];      // D2H Register FIS
    uint8_t        res3  [0x04];
    uint8_t        sdbfis[0x08];    // Set Device Bits FIS
    uint8_t        ufis  [0x40];      // Unknown FIS
    uint8_t        res4  [0x60];
} __attribute__((packed)) fis;

typedef enum {
    FIS_TYPE_REG_H2D    = 0x27,    // Register FIS - host to device
    FIS_TYPE_REG_D2H    = 0x34,    // Register FIS - device to host
    FIS_TYPE_DMA_ACT    = 0x39,    // DMA activate FIS - device to host
    FIS_TYPE_DMA_SETUP  = 0x41,    // DMA setup FIS - bidirectional
    FIS_TYPE_DATA       = 0x46,    // Data FIS - bidirectional
    FIS_TYPE_BIST       = 0x58,    // BIST activate FIS - bidirectional
    FIS_TYPE_PIO_SETUP  = 0x5F,    // PIO setup FIS - device to host
    FIS_TYPE_DEV_BITS   = 0xA1,    // Set device bits FIS - device to host
} FIS_TYPE;

typedef struct tagFIS_REG_H2D {
    uint8_t    fis_type;   // FIS_TYPE_REG_H2D
    uint8_t    pmport:4;   // Port multiplier
    uint8_t    rsv0:3;     // Reserved
    uint8_t    c:1;        // 1: Command, 0: Control
    uint8_t    command;    // Command register
    uint8_t    featurel;   // Feature register, 7:0
    uint8_t    lba0;       // LBA low register, 7:0
    uint8_t    lba1;       // LBA mid register, 15:8
    uint8_t    lba2;       // LBA high register, 23:16
    uint8_t    device;     // Device register
    uint8_t    lba3;       // LBA register, 31:24
    uint8_t    lba4;       // LBA register, 39:32
    uint8_t    lba5;       // LBA register, 47:40
    uint8_t    featureh;   // Feature register, 15:8
    uint8_t    countl;     // Count register, 7:0
    uint8_t    counth;     // Count register, 15:8
    uint8_t    icc;        // Isochronous command completion
    uint8_t    control;    // Control register
    uint8_t    rsv1[4];    // Reserved
} FIS_REG_H2D;

typedef volatile struct tagHBA_MEM {
    // 0x00 - 0x2B, Generic Host Control
    uint32_t    cap;        // 0x00, Host capability
    uint32_t    ghc;        // 0x04, Global host control
    uint32_t    is;         // 0x08, Interrupt status
    uint32_t    pi;         // 0x0C, Port implemented
    uint32_t    vs;         // 0x10, Version
    uint32_t    ccc_ctl;    // 0x14, Command completion coalescing control
    uint32_t    ccc_pts;    // 0x18, Command completion coalescing ports
    uint32_t    em_loc;        // 0x1C, Enclosure management location
    uint32_t    em_ctl;        // 0x20, Enclosure management control
    uint32_t    cap2;        // 0x24, Host capabilities extended
    uint32_t    bohc;        // 0x28, BIOS/OS handoff control and status
    
    // 0x2C - 0x9F, Reserved
    uint8_t    rsv[0xA0-0x2C];
    
    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t    vendor[0x100-0xA0];
    
    // 0x100 - 0x10FF, Port control registers
    HBA_PORT    ports[1];    // 1 ~ 32
} HBA_MEM;

typedef struct tagHBA_CMD_HEADER {
    // DW0
    uint8_t    cfl:5;      // Command FIS length in uint32_tS, 2 ~ 16
    uint8_t    a:1;        // ATAPI
    uint8_t    w:1;        // Write, 1: H2D, 0: D2H
    uint8_t    p:1;        // Prefetchable
    
    uint8_t    r:1;        // Reset
    uint8_t    b:1;        // BIST
    uint8_t    c:1;        // Clear busy upon R_OK
    uint8_t    rsv0:1;     // Reserved
    uint8_t    pmp:4;      // Port multiplier port
    
    uint16_t    prdtl;      // Physical region descriptor table length in entries
    
    // DW1
    volatile
    uint32_t   prdbc;      // Physical region descriptor uint8_t count transferred
    
    // DW2, 3
    uint32_t   ctba;       // Command table descriptor base address
    uint32_t   ctbau;      // Command table descriptor base address upper 32 bits
    
    // DW4 - 7
    uint32_t   rsv1[4];    // Reserved
} HBA_CMD_HEADER;

typedef struct tagHBA_PRDT_ENTRY {
    uint32_t dba;        // Data base address
    uint32_t dbau;       // Data base address upper 32 bits
    uint32_t rsv0;       // Reserved
    
    // DW3
    uint32_t dbc:22;     // Byte count, 4M max
    uint32_t rsv1:9;     // Reserved
    uint32_t i:1;        // Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL {
    // 0x00
    uint8_t  cfis[64];    // Command FIS
    
    // 0x40
    uint8_t  acmd[16];    // ATAPI command, 12 or 16 bytes
    
    // 0x50
    uint8_t  rsv[48];     // Reserved
    
    // 0x80
    HBA_PRDT_ENTRY prdt_entry[1];    // Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;


struct x86_64_context_switch_frame {
    uint64_t r15, r14, r13, r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rip;
};

struct context_switch_regs {
    uint64_t es;
    uint64_t ds;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rbp;
//    uint64_t fs;
//    uint64_t gs;
    uint64_t rip;
    uint64_t cs;
    uint64_t eflags;
    uint64_t rsp;
    uint64_t ss;
};

struct e1000_rx_descriptor {
    uint64_t            buffer;
    uint16_t            length;
    uint16_t            padding;
    uint8_t             status;
    uint8_t             error;
    uint16_t            padding2;
} __attribute__((packed)) __attribute__((aligned (4)));

struct e1000_tx_descriptor {
    uint64_t            buffer;
    uint16_t            length;
    uint8_t             checksum_offset;
    uint8_t             cmd;
    uint8_t             status;
    uint8_t             checksum_start;
    uint16_t            special;
} __attribute__((packed)) __attribute__((aligned (4)));

typedef struct {
    SMBIOSHeader * SMBIOS;
    unsigned long  OriginalAddress;
    bool           foundInBios;
} SMBIOS_for_Swift;

typedef struct {
    ACPI_2_Description *RSDP;
    unsigned long       OriginalAddress;
    bool                foundInBios;
} RSDP_for_Swift;

static inline void hlt() {
    pal_hlt();
}
static inline void sti() {
    pal_sti();
}
static inline void cli() {
    pal_cli();
}

// VMware Tools Backdoor

#define BackdoorMagic 0x564D5868
#define BackdoorPort  0x5658
#define BackdoorPortHighBandwidth 0x5659

#define BDOOR_CMD_GETMHZ                1
#define BDOOR_CMD_APMFUNCTION           2
#define BDOOR_CMD_GETDISKGEO            3
#define BDOOR_CMD_GETPTRLOCATION        4
#define BDOOR_CMD_SETPTRLOCATION        5
#define BDOOR_CMD_GETSELLENGTH          6
#define BDOOR_CMD_GETNEXTPIECE          7
#define BDOOR_CMD_SETSELLENGTH          8
#define BDOOR_CMD_SETNEXTPIECE          9
#define BDOOR_CMD_GETVERSION            10
#define BDOOR_CMD_GETDEVICELISTELEMENT  11
#define BDOOR_CMD_TOGGLEDEVICE          12
#define BDOOR_CMD_GETGUIOPTIONS         13
#define BDOOR_CMD_SETGUIOPTIONS         14
#define BDOOR_CMD_GETSCREENSIZE         15
#define BDOOR_CMD_MONITOR_CONTROL       16
#define BDOOR_CMD_GETHWVERSION          17
#define BDOOR_CMD_OSNOTFOUND            18
#define BDOOR_CMD_GETUUID               19
#define BDOOR_CMD_GETMEMSIZE            20
#define BDOOR_CMD_HOSTCOPY              21 /* Devel only */
#define BDOOR_CMD_SERVICE_VM            22 /* prototype only */
#define BDOOR_CMD_GETTIME               23 /* Deprecated. Use GETTIMEFULL. */
#define BDOOR_CMD_STOPCATCHUP           24
#define BDOOR_CMD_PUTCHR                25 /* Devel only */
#define BDOOR_CMD_ENABLE_MSG            26 /* Devel only */
#define BDOOR_CMD_GOTO_TCL              27 /* Devel only */
#define BDOOR_CMD_INITPCIOPROM          28
#define BDOOR_CMD_INT13                 29
#define BDOOR_CMD_MESSAGE               30
#define BDOOR_CMD_RSVD0                 31
#define BDOOR_CMD_RSVD1                 32
#define BDOOR_CMD_RSVD2                 33
#define BDOOR_CMD_ISACPIDISABLED        34
#define BDOOR_CMD_TOE                   35 /* Not in use */
#define BDOOR_CMD_ISMOUSEABSOLUTE       36
#define BDOOR_CMD_PATCH_SMBIOS_STRUCTS  37
#define BDOOR_CMD_MAPMEM                38 /* Devel only */
#define BDOOR_CMD_ABSPOINTER_DATA       39
#define BDOOR_CMD_ABSPOINTER_STATUS     40
#define BDOOR_CMD_ABSPOINTER_COMMAND    41
#define BDOOR_CMD_TIMER_SPONGE          42
#define BDOOR_CMD_PATCH_ACPI_TABLES     43
#define BDOOR_CMD_DEVEL_FAKEHARDWARE    44 /* Debug only - needed in beta */
#define BDOOR_CMD_GETHZ                 45
#define BDOOR_CMD_GETTIMEFULL           46
#define BDOOR_CMD_STATELOGGER           47
#define BDOOR_CMD_CHECKFORCEBIOSSETUP   48
#define BDOOR_CMD_LAZYTIMEREMULATION    49
#define BDOOR_CMD_BIOSBBS               50
#define BDOOR_CMD_VASSERT               51
#define BDOOR_CMD_ISGOSDARWIN           52
#define BDOOR_CMD_DEBUGEVENT            53
#define BDOOR_CMD_OSNOTMACOSXSERVER     54
#define BDOOR_CMD_GETTIMEFULL_WITH_LAG  55
#define BDOOR_CMD_ACPI_HOTPLUG_DEVICE   56
#define BDOOR_CMD_ACPI_HOTPLUG_MEMORY   57
#define BDOOR_CMD_ACPI_HOTPLUG_CBRET    58
#define BDOOR_CMD_GET_HOST_VIDEO_MODES  59
#define BDOOR_CMD_ACPI_HOTPLUG_CPU      60
#define BDOOR_CMD_MAX                   61

#define BDOORHB_CMD_MESSAGE 0
#define BDOORHB_CMD_VASSERT 1
#define BDOORHB_CMD_MAX 2

static inline void BackdoorIn(uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx, uint32_t* rsi, uint32_t* rdi) {
    *rax = BackdoorMagic;
    *rdx = (*rdx & 0xFFFF0000) | BackdoorPort;
    __asm__ volatile ("in %%dx, %0" : "+a" (*rax), "+b" (*rbx), "+c" (*rcx), "+d" (*rdx), "+S" (*rsi), "+D" (*rdi));
}

static inline void BackdoorHighBandwidthOut(uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx, uint32_t* rsi, uint32_t* rdi) {
    *rax = BackdoorMagic;
    *rdx = (*rdx & 0xFFFF0000) | BackdoorPortHighBandwidth;
    __asm__ volatile ("cld; rep; outsb" : "+a" (*rax), "+b" (*rbx), "+c" (*rcx), "+d" (*rdx), "+S" (*rsi), "+D" (*rdi));
}

static inline void BackdoorHighBandwidthIn(uint32_t* rax, uint32_t* rbx, uint32_t* rcx, uint32_t* rdx, uint32_t* rsi, uint32_t* rdi) {
    *rax = BackdoorMagic;
    *rdx = (*rdx & 0xFFFF0000) | BackdoorPortHighBandwidth;
    __asm__ volatile ("cld; rep; insb" : "+a" (*rax), "+b" (*rbx), "+c" (*rcx), "+d" (*rdx), "+S" (*rsi), "+D" (*rdi));
}

extern void testObjc(void);
extern RSDP_for_Swift RSDP_;
extern SMBIOS_for_Swift SMBIOS_;
extern Platform_state_t  Platform_state;
extern bool VendorisApple;
extern bool modified;
extern bool experimental;
extern bool early;
extern bool beforeInited;
extern int cliCalled;
extern bool canUseSSEmemcpy;
extern bool kRebootOnPanic;
extern unsigned long inline ml_static_ptovirt(unsigned long paddr);
extern void ModulesStartController(void);
extern void PrintLoadedModules(void);
extern void IncommingInterrupt(int InterruptNumber);
extern unsigned long long io_map(unsigned long long phys_addr, unsigned long size, unsigned int flags);
static inline unsigned int
GetBaseAddr(unsigned char Bus, unsigned char Slot, unsigned char Function) {
    return (unsigned int)(Bus) << 16 | 0x80000000 | (unsigned int)(Slot) << 11 | (unsigned int)(Function) << 8;
}
extern void
change_color(unsigned int foreground, unsigned int background);
extern void APICInit(void);
extern void lapic_init(void);
extern void reboot_system(bool ispanic);
extern void shutdown_system();
extern void serial_putc(int c);
extern void clear_screen(void);
extern void
x86_64_context_switch(uintptr_t *oldsp, uintptr_t *newsp);
extern void
x86_64_context_switch_first(uintptr_t* newsp);
extern void
printf_state_swift(x86_saved_state64_t saved_state);

extern unsigned long Screen;
extern void paint_char(unsigned int x, unsigned int y, unsigned char ch);
