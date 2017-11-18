//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#include <version.h>
#include <kernel/command.h>
#include <platform/platform.h>

#include <i386/pio.h>
#include <i386/pal.h>
#include <i386/cpuid.h>


#ifdef DEBUG
#define BUILD_TYPE "Internal"
#else
#define BUILD_TYPE ""
#endif

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

extern RSDP_for_Swift RSDP_;
extern SMBIOS_for_Swift SMBIOS_;
extern Platform_state_t  Platform_state;
extern unsigned long inline ml_static_ptovirt(unsigned long paddr);
extern void ModulesStartController(void);
extern void PrintLoadedModules(void);
extern unsigned long long io_map(unsigned long long phys_addr, unsigned long size, unsigned int flags);
static inline unsigned int
GetBaseAddr(unsigned char Bus, unsigned char Slot, unsigned char Function) {
    return (unsigned int)(Bus) << 16 | 0x80000000 | (unsigned int)(Slot) << 11 | (unsigned int)(Function) << 8;
}
extern void
change_color(unsigned int foreground, unsigned int background);
extern void
change_font_color(unsigned int foreground);
extern unsigned int get_font_color(void);
extern void
paint_rectangle(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char width, unsigned char height);
