//
//  i386_vm_init.c
//  BetaOS
//
//  Created by Adam Kopeć on 7/24/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include "cpuid.h"
#include "cpu_data.h"
#include "proc_reg.h"
#include "pmap.h"
#include "thread.h"
#include "vm_param.h"
#include "machine_routines.h"

#include "misc_protos.h"

#include <kernel/kernel_header.h>

#include <platform/platform.h>
#include <platform/boot.h>
#include <platform/efi.h>

#include <x86_64/lowglobals.h>
#include <stdio.h>

#ifndef DBG
#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif
#endif

uint8_t __attribute__((section("__DATA,__const"))) dataconst = 0xFF; // Just to fill __DATA,__const

// Temp

/*
 *	The virtual page size is currently implemented as a runtime
 *	variable, but is constant once initialized using vm_set_page_size.
 *	This initialization must be done in the machine-dependent
 *	bootstrap sequence, before calling other machine-independent
 *	initializations.
 *
 *	All references to the virtual page size outside this
 *	module must use the PAGE_SIZE, PAGE_MASK and PAGE_SHIFT
 *	constants.
 */
vm_size_t	vm_page_size  = PAGE_SIZE;
vm_size_t	vm_page_mask  = PAGE_MASK;
int         vm_page_shift = PAGE_SHIFT;

/*
 *	vm_set_page_size:
 *
 *	Sets the page size, perhaps based upon the memory
 *	size.  Must be called before any use of page-size
 *	dependent functions.
 *
 *	Sets page_shift and page_mask from page_size.
 */
void
vm_set_page_size(void) {
    vm_page_size  = PAGE_SIZE;
    vm_page_mask  = PAGE_MASK;
    vm_page_shift = PAGE_SHIFT;
    
    if ((vm_page_mask & vm_page_size) != 0)
        panic("vm_set_page_size: page size not a power of two");
    
    for (vm_page_shift = 0; ; vm_page_shift++)
        if ((1U << vm_page_shift) == vm_page_size)
            break;
}




/*
 0 = all pages avail ( default. )
 1 = disable high mem ( cap max pages to 4G)
 2 = prefer himem
 */
extern int		vm_himemory_mode;

extern bool         vm_lopage_needed;
extern uint32_t		vm_lopage_free_count;
extern uint32_t		vm_lopage_free_limit;
extern uint32_t		vm_lopage_lowater;
extern bool         vm_lopage_refill;
extern uint64_t		max_valid_dma_address;
extern ppnum_t		max_valid_low_ppnum;

vm_size_t       mem_size    = 0;
pmap_paddr_t	first_avail = 0;    /* first after page tables */

uint64_t        max_mem;            /* Size of physical memory (bytes), adjusted by maxmem */
uint64_t        mem_actual;
uint64_t        sane_size = 0;      /* Memory size for defaults calculations */

/*
 * KASLR parameters
 */
ppnum_t		vm_kernel_base_page;
vm_offset_t	vm_kernel_base;
vm_offset_t	vm_kernel_top;
vm_offset_t	vm_kernel_stext;
vm_offset_t	vm_kernel_etext;
vm_offset_t	vm_kernel_slide;
vm_offset_t vm_hib_base;
vm_offset_t	vm_kext_base = VM_MIN_KERNEL_AND_KEXT_ADDRESS;
vm_offset_t	vm_kext_top  = VM_MIN_KERNEL_ADDRESS;

vm_offset_t vm_prelink_stext;
vm_offset_t vm_prelink_etext;
vm_offset_t vm_prelink_sinfo;
vm_offset_t vm_prelink_einfo;
vm_offset_t vm_slinkedit;
vm_offset_t vm_elinkedit;

#define MAXLORESERVE	(32 * 1024 * 1024)

ppnum_t		max_ppnum  = 0;
ppnum_t		lowest_lo  = 0;
ppnum_t		lowest_hi  = 0;
ppnum_t		highest_hi = 0;

enum {PMAP_MAX_RESERVED_RANGES = 32};
uint32_t pmap_reserved_pages_allocated = 0;
uint32_t pmap_reserved_range_indices[PMAP_MAX_RESERVED_RANGES];
uint32_t pmap_last_reserved_range_index = 0;
uint32_t pmap_reserved_ranges = 0;

extern unsigned int bsd_mbuf_cluster_reserve(bool *);

pmap_paddr_t        avail_start, avail_end;
vm_offset_t         virtual_avail, virtual_end;
static pmap_paddr_t	avail_remaining;
vm_offset_t         static_memory_end = 0;

vm_offset_t	sHIB, eHIB, stext, etext, sdata, edata, sconstdata, econstdata, end;

/*
 * _mh_execute_header is the mach_header for the currently executing kernel
 */
vm_offset_t segTEXTB; unsigned long segSizeTEXT;
vm_offset_t segDATAB; unsigned long segSizeDATA;
vm_offset_t segLINKB; unsigned long segSizeLINK;
vm_offset_t segPRELINKB; unsigned long segSizePRELINK;
vm_offset_t segPRELINKINFOB; unsigned long segSizePRELINKINFO;
vm_offset_t segHIBB; unsigned long segSizeHIB;
vm_offset_t sectCONSTB; unsigned long sectSizeConst;

bool        doconstro_override = false;

static kernel_segment_command_t *segTEXT, *segDATA;
static kernel_section_t         *cursectTEXT, *lastsectTEXT;
static kernel_section_t         *sectDCONST;

/*extern uint64_t firmware_Conventional_bytes;
extern uint64_t firmware_RuntimeServices_bytes;
extern uint64_t firmware_ACPIReclaim_bytes;
extern uint64_t firmware_ACPINVS_bytes;
extern uint64_t firmware_PalCode_bytes;
extern uint64_t firmware_Reserved_bytes;
extern uint64_t firmware_Unusable_bytes;
extern uint64_t firmware_other_bytes;*/
uint64_t firmware_Conventional_bytes = 0;
uint64_t firmware_RuntimeServices_bytes = 0;
uint64_t firmware_ACPIReclaim_bytes = 0;
uint64_t firmware_ACPINVS_bytes = 0;
uint64_t firmware_PalCode_bytes = 0;
uint64_t firmware_Reserved_bytes = 0;
uint64_t firmware_Unusable_bytes = 0;
uint64_t firmware_other_bytes = 0;
uint64_t firmware_MMIO_bytes;

/*
 * Linker magic to establish the highest address in the kernel.
 */
extern void 	*last_kernel_symbol;

/*
 * Basic VM initialization.
 */
void
i386_vm_init(uint64_t maxmem, bool IA32e, boot_args *args) {
    pmap_memory_region_t *pmptr;
    pmap_memory_region_t *prev_pmptr;
    EfiMemoryRange       *mptr;
    unsigned int mcount;
    unsigned int msize;
    ppnum_t      fap;
    unsigned int i;
    ppnum_t     maxpg = 0;
    uint32_t    pmap_type;
    uint32_t    maxloreserve;
    //uint32_t    maxdmaaddr;
    uint32_t    mbuf_reserve  = 0;
    bool        mbuf_override = false;
    bool        coalescing_permitted;
    vm_kernel_base_page = i386_btop(args->kaddr);
    vm_offset_t base_address;
    vm_offset_t static_base_address;
    
    /*
     * Establish the KASLR parameters.
     */
    static_base_address = ml_static_ptovirt(KERNEL_BASE_OFFSET);
    base_address        = ml_static_ptovirt(args->kaddr);
    vm_kernel_slide     = base_address - static_base_address;
    if (args->kslide) {
        printf("KASLR slide: 0x%016llx dynamic\n", vm_kernel_slide);
        if (vm_kernel_slide != ((vm_offset_t)args->kslide))
            panic("Kernel base inconsistent with slide - rebased?");
    } else {
        /* No slide relative to on-disk symbols */
        printf("KASLR slide: 0x%016llx static and ignored\n", vm_kernel_slide);
        vm_kernel_slide = 0;
    }
    
    if (_mh_execute_header.flags & MH_PIE) {
        struct load_command *loadcmd;
        uint32_t cmd;
        
        loadcmd = (struct load_command *)((uintptr_t)&_mh_execute_header + sizeof (_mh_execute_header));
        
        for (cmd = 0; cmd < _mh_execute_header.ncmds; cmd++) {
            if (loadcmd->cmd == LC_DYSYMTAB) {
                struct dysymtab_command *dysymtab;
                
                dysymtab = (struct dysymtab_command *)loadcmd;
                dysymtab->nlocrel = 0;
                dysymtab->locreloff = 0;
                printf("Hiding local relocations\n");
                break;
            }
            loadcmd = (struct load_command *)((uintptr_t)loadcmd + loadcmd->cmdsize);
        }
    }
    
    /*
     * Now retrieve addresses for end, edata, and etext
     * from MACH-O headers.
     */
    segTEXTB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__TEXT", &segSizeTEXT);
    segDATAB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__DATA", &segSizeDATA);
    segLINKB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__LINKEDIT", &segSizeLINK);
    segHIBB  = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__HIB", &segSizeHIB);
    segPRELINKB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__PRELINK_TEXT", &segSizePRELINK);
    segPRELINKINFOB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header, "__PRELINK_INFO", &segSizePRELINKINFO);
    segTEXT = getsegbynamefromheader(&_mh_execute_header, "__TEXT");
    segDATA = getsegbynamefromheader(&_mh_execute_header, "__DATA");
    sectDCONST = (const struct section_64 *)getsectbynamefromheader((const struct mach_header *)&_mh_execute_header, "__DATA", "__const");
    cursectTEXT = lastsectTEXT = firstsect(segTEXT);
    /* Discover the last TEXT section within the TEXT segment */
    while ((cursectTEXT = nextsect(segTEXT, cursectTEXT)) != NULL) {
        lastsectTEXT = cursectTEXT;
    }
    
    sHIB  = segHIBB;
    
    eHIB  = segHIBB + segSizeHIB;
    vm_hib_base = sHIB;
    /* Zero-padded from ehib to stext if text is 2M-aligned */
    stext = segTEXTB;
    lowGlo.lgStext = stext;
    
    etext = (vm_offset_t) round_page_64(lastsectTEXT->addr + lastsectTEXT->size);
    /* Zero-padded from etext to sdata if text is 2M-aligned */
    sdata = segDATAB;
    edata = segDATAB + segSizeDATA;
    
    sectCONSTB = (vm_offset_t) sectDCONST->addr;
    sectSizeConst = sectDCONST->size;
    sconstdata = sectCONSTB;
    econstdata = sectCONSTB + sectSizeConst;
    
    if (sectSizeConst & PAGE_MASK) {
        kernel_section_t *ns = nextsect(segDATA, sectDCONST);
        if (ns && !(ns->addr & PAGE_MASK))
            doconstro_override = true;
    } else
        doconstro_override = true;
    
    DBG("segTEXTB    = %p\n", (void *) segTEXTB);
    DBG("segDATAB    = %p\n", (void *) segDATAB);
    DBG("segLINKB    = %p\n", (void *) segLINKB);
    DBG("segHIBB     = %p\n", (void *) segHIBB);
    DBG("segPRELINKB = %p\n", (void *) segPRELINKB);
    DBG("segPRELINKINFOB = %p\n", (void *) segPRELINKINFOB);
    DBG("sHIB        = %p\n", (void *) sHIB);
    DBG("eHIB        = %p\n", (void *) eHIB);
    DBG("stext       = %p\n", (void *) stext);
    DBG("etext       = %p\n", (void *) etext);
    DBG("sdata       = %p\n", (void *) sdata);
    DBG("edata       = %p\n", (void *) edata);
    DBG("sconstdata  = %p\n", (void *) sconstdata);
    DBG("econstdata  = %p\n", (void *) econstdata);
    //DBG("kernel_top  = %p\n", (void *) &last_kernel_symbol);
    
    vm_kernel_base  = sHIB;
    //vm_kernel_top   = (vm_offset_t) &last_kernel_symbol;
    vm_kernel_stext = stext;
    vm_kernel_etext = etext;
    
    vm_prelink_stext = segPRELINKB;
    vm_prelink_etext = segPRELINKB + segSizePRELINK;
    vm_prelink_sinfo = segPRELINKINFOB;
    vm_prelink_einfo = segPRELINKINFOB + segSizePRELINKINFO;
    vm_slinkedit = segLINKB;
    vm_elinkedit = segLINKB + segSizePRELINK;
    
    vm_set_page_size();
    
    /*
     * Compute the memory size.
     */
    
    avail_remaining = 0;
    avail_end = 0;
    pmptr = pmap_memory_regions;
    prev_pmptr = 0;
    pmap_memory_region_count = pmap_memory_region_current = 0;
    fap = (ppnum_t) i386_btop(first_avail);
    
    mptr = (EfiMemoryRange *)ml_static_ptovirt((vm_offset_t)args->MemoryMap);
    if (args->MemoryMapDescriptorSize == 0)
        panic("Invalid memory map descriptor size");
    msize = args->MemoryMapDescriptorSize;
    mcount = args->MemoryMapSize / msize;
    
#define FOURGIG 0x0000000100000000ULL
#define ONEGIG  0x0000000040000000ULL
    
    for (i = 0; i < mcount; i++, mptr = (EfiMemoryRange *)(((vm_offset_t)mptr) + msize)) {
        ppnum_t  base, top;
        uint64_t region_bytes = 0;
        
        if (pmap_memory_region_count >= PMAP_MEMORY_REGIONS_SIZE) {
            printf("WARNING: truncating memory region count at %d\n", pmap_memory_region_count);
            break;
        }
        base = (ppnum_t)   (mptr->PhysicalStart  >> I386_PGSHIFT);
        top  = (ppnum_t) (((mptr->PhysicalStart) >> I386_PGSHIFT) + mptr->NumberOfPages - 1);
        
        if (base == 0) {
            /*
             * Avoid having to deal with the edge case of the
             * very first possible physical page and the roll-over
             * to -1; just ignore that page.
             */
            printf("WARNING: ignoring first page in [0x%llx:0x%llx]\n", (uint64_t) base, (uint64_t) top);
            base++;
        }
        if (top + 1 == 0) {
            /*
             * Avoid having to deal with the edge case of the
             * very last possible physical page and the roll-over
             * to 0; just ignore that page.
             */
            printf("WARNING: ignoring last page in [0x%llx:0x%llx]\n", (uint64_t) base, (uint64_t) top);
            top--;
        }
        if (top < base) {
            /*
             * That was the only page in that region, so
             * ignore the whole region.
             */
            continue;
        }
#if	MR_RSV_TEST
        static uint32_t nmr = 0;
        if ((base > 0x20000) && (nmr++ < 4))
            mptr->Attribute |= EFI_MEMORY_KERN_RESERVED;
#endif
        region_bytes = (uint64_t)(mptr->NumberOfPages << I386_PGSHIFT);
        pmap_type = mptr->Type;
        
        switch (mptr->Type) {
            case kEfiLoaderCode:
            case kEfiLoaderData:
            case kEfiBootServicesCode:
            case kEfiBootServicesData:
            case kEfiConventionalMemory:
                /*
                 * Consolidate usable memory types into one.
                 */
                pmap_type = kEfiConventionalMemory;
                sane_size += region_bytes;
                firmware_Conventional_bytes += region_bytes;
                break;
                /*
                 * sane_size should reflect the total amount of physical
                 * RAM in the system, not just the amount that is
                 * available for the OS to use.
                 * We now get this value from SMBIOS tables
                 * rather than reverse engineering the memory map.
                 * But the legacy computation of "sane_size" is kept
                 * for diagnostic information.
                 */
                
            case kEfiRuntimeServicesCode:
            case kEfiRuntimeServicesData:
                firmware_RuntimeServices_bytes += region_bytes;
                sane_size += region_bytes;
                break;
            case kEfiACPIReclaimMemory:
                firmware_ACPIReclaim_bytes += region_bytes;
                sane_size += region_bytes;
                break;
            case kEfiACPIMemoryNVS:
                firmware_ACPINVS_bytes += region_bytes;
                sane_size += region_bytes;
                break;
            case kEfiPalCode:
                firmware_PalCode_bytes += region_bytes;
                sane_size += region_bytes;
                break;
                
            case kEfiReservedMemoryType:
                firmware_Reserved_bytes += region_bytes;
                break;
            case kEfiUnusableMemory:
                firmware_Unusable_bytes += region_bytes;
                break;
            case kEfiMemoryMappedIO:
            case kEfiMemoryMappedIOPortSpace:
                firmware_MMIO_bytes += region_bytes;
                break;
            default:
                firmware_other_bytes += region_bytes;
                break;
        }
        
        DBG("EFI region %d: type %u/%d, base 0x%x, top 0x%x %s\n", i, mptr->Type, pmap_type, base, top, (mptr->Attribute&EFI_MEMORY_KERN_RESERVED)? "RESERVED" : (mptr->Attribute&EFI_MEMORY_RUNTIME)? "RUNTIME" : "");
        
        if (maxpg) {
            if (base >= maxpg)
                break;
            top = (top > maxpg) ? maxpg : top;
        }
        
        /*
         * handle each region
         */
        if ((mptr->Attribute & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME ||
            pmap_type != kEfiConventionalMemory) {
            prev_pmptr = 0;
            continue;
        } else {
            /*
             * Usable memory region
             */
            if (top < I386_LOWMEM_RESERVED) {
                prev_pmptr = 0;
                continue;
            }
            /*
             * A range may be marked with with the
             * EFI_MEMORY_KERN_RESERVED attribute
             * on some systems, to indicate that the range
             * must not be made available to devices.
             */
            
            if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED) {
                if (++pmap_reserved_ranges > PMAP_MAX_RESERVED_RANGES) {
                    panic("Too many reserved ranges %u\n", pmap_reserved_ranges);
                }
            }
            
            if (top < fap) {
                /*
                 * entire range below first_avail
                 * salvage some low memory pages
                 * we use some very low memory at startup
                 * mark as already allocated here
                 */
                if (base >= I386_LOWMEM_RESERVED)
                    pmptr->base = base;
                else
                    pmptr->base = I386_LOWMEM_RESERVED;
                
                pmptr->end = top;
                
                
                if ((mptr->Attribute & EFI_MEMORY_KERN_RESERVED) &&
                    (top < vm_kernel_base_page)) {
                    pmptr->alloc_up   = pmptr->base;
                    pmptr->alloc_down = pmptr->end;
                    pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
                }
                else {
                    /*
                     * mark as already mapped
                     */
                    pmptr->alloc_up = top + 1;
                    pmptr->alloc_down = top;
                }
                pmptr->type = pmap_type;
                pmptr->attribute = mptr->Attribute;
            }
            else if ( (base < fap) && (top > fap) ) {
                /*
                 * spans first_avail
                 * put mem below first avail in table but
                 * mark already allocated
                 */
                pmptr->base         = base;
                pmptr->end          = (fap - 1);
                pmptr->alloc_up     = pmptr->end + 1;
                pmptr->alloc_down   = pmptr->end;
                pmptr->type         = pmap_type;
                pmptr->attribute    = mptr->Attribute;
                /*
                 * we bump these here inline so the accounting
                 * below works correctly
                 */
                pmptr++;
                pmap_memory_region_count++;
                
                pmptr->alloc_up     = pmptr->base = fap;
                pmptr->type         = pmap_type;
                pmptr->attribute    = mptr->Attribute;
                pmptr->alloc_down   = pmptr->end = top;
                
                if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED)
                    pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
            } else {
                /*
                 * entire range useable
                 */
                pmptr->alloc_up     = pmptr->base = base;
                pmptr->type         = pmap_type;
                pmptr->attribute    = mptr->Attribute;
                pmptr->alloc_down   = pmptr->end = top;
                if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED)
                    pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
            }
            
            if (i386_ptob(pmptr->end) > avail_end )
                avail_end = i386_ptob(pmptr->end);
            
            avail_remaining += (pmptr->end - pmptr->base);
            coalescing_permitted = (prev_pmptr && (pmptr->attribute == prev_pmptr->attribute) && ((pmptr->attribute & EFI_MEMORY_KERN_RESERVED) == 0));
            /*
             * Consolidate contiguous memory regions, if possible
             */
            if (prev_pmptr &&
                (pmptr->type == prev_pmptr->type) &&
                (coalescing_permitted) &&
                (pmptr->base == pmptr->alloc_up) &&
                (prev_pmptr->end == prev_pmptr->alloc_down) &&
                (pmptr->base == (prev_pmptr->end + 1)))
            {
                prev_pmptr->end         = pmptr->end;
                prev_pmptr->alloc_down  = pmptr->alloc_down;
            } else {
                pmap_memory_region_count++;
                prev_pmptr = pmptr;
                pmptr++;
            }
        }
    }
    
#ifdef DEBUG
    {
        unsigned int j;
        pmap_memory_region_t *p = pmap_memory_regions;
        uint64_t region_start, region_end;
        uint64_t efi_start, efi_end;
        for (j=0;j<pmap_memory_region_count;j++, p++) {
            printf("pmap region %d type %d base 0x%llx alloc_up 0x%llx alloc_down 0x%llx top 0x%llx\n",
                    j, p->type,
                    (uint64_t) p->base  << I386_PGSHIFT,
                    (uint64_t) p->alloc_up << I386_PGSHIFT,
                    (uint64_t) p->alloc_down << I386_PGSHIFT,
                    (uint64_t) p->end   << I386_PGSHIFT);
            region_start = (uint64_t) p->base << I386_PGSHIFT;
            region_end = ((uint64_t) p->end << I386_PGSHIFT) - 1;
            mptr = (EfiMemoryRange *) ml_static_ptovirt((vm_offset_t)args->MemoryMap);
            for (i=0; i<mcount; i++, mptr = (EfiMemoryRange *)(((vm_offset_t)mptr) + msize)) {
                if (mptr->Type != kEfiLoaderCode &&
                    mptr->Type != kEfiLoaderData &&
                    mptr->Type != kEfiBootServicesCode &&
                    mptr->Type != kEfiBootServicesData &&
                    mptr->Type != kEfiConventionalMemory) {
                    efi_start = (uint64_t)mptr->PhysicalStart;
                    efi_end = efi_start + ((vm_offset_t)mptr->NumberOfPages << I386_PGSHIFT) - 1;
                    if ((efi_start >= region_start && efi_start <= region_end) ||
                        (efi_end >= region_start && efi_end <= region_end)) {
                        printf(" *** Overlapping region with EFI runtime region %d\n", i);
                    }
                }
            }
        }
    }
#endif
    
    avail_start = first_avail;
    mem_actual = args->PhysicalMemorySize;
    
    /*
     * For user visible memory size, round up to 128 Mb
     * - accounting for the various stolen memory not reported by EFI.
     * This is maintained for historical, comparison purposes but
     * we now use the memory size reported by EFI/Booter.
     */
    sane_size = (sane_size + 128 * MB - 1) & ~((uint64_t)(128 * MB - 1));
    if (sane_size != mem_actual)
        printf("mem_actual: 0x%llx\n legacy sane_size: 0x%llx\n", mem_actual, sane_size);
    sane_size = mem_actual;
    
    /*
     * We cap at KERNEL_MAXMEM bytes. Unless overriden by the maxmem boot-arg
     * -- which is a non-zero maxmem argument to this function.
     */
    if (maxmem == 0 && sane_size > KERNEL_MAXMEM) {
        maxmem = KERNEL_MAXMEM;
        printf("Physical memory %lld bytes capped at %dGB\n", sane_size, (uint32_t) (KERNEL_MAXMEM/GB));
    }
    
    /*
     * if user set maxmem, reduce memory sizes
     */
    if ((maxmem > (uint64_t)first_avail) && (maxmem < sane_size)) {
        ppnum_t discarded_pages  = (ppnum_t)((sane_size - maxmem) >> I386_PGSHIFT);
        ppnum_t	highest_pn  = 0;
        ppnum_t	cur_end     = 0;
        uint64_t pages_to_use;
        unsigned cur_region = 0;
        
        sane_size = maxmem;
        
        if (avail_remaining > discarded_pages)
            avail_remaining -= discarded_pages;
        else
            avail_remaining = 0;
        
        pages_to_use = avail_remaining;
        
        while (cur_region < pmap_memory_region_count && pages_to_use) {
            for (cur_end = pmap_memory_regions[cur_region].base;
                 cur_end < pmap_memory_regions[cur_region].end && pages_to_use;
                 cur_end++) {
                if (cur_end > highest_pn)
                    highest_pn = cur_end;
                pages_to_use--;
            }
            if (pages_to_use == 0) {
                pmap_memory_regions[cur_region].end = cur_end;
                pmap_memory_regions[cur_region].alloc_down = cur_end;
            }
            
            cur_region++;
        }
        pmap_memory_region_count = cur_region;
        
        avail_end = i386_ptob(highest_pn + 1);
    }
    
    if (sane_size > (FOURGIG >> 1))
        mem_size = (vm_size_t)(FOURGIG >> 1);
    else
        mem_size = (vm_size_t)sane_size;
    max_mem = sane_size;
    
    printf("Physical memory %llu MB\n", sane_size/MB);
    
    max_valid_low_ppnum = (2 * GB) / PAGE_SIZE;
    //if (!PE_parse_boot_argn("max_valid_dma_addr", &maxdmaaddr, sizeof (maxdmaaddr))) {
        max_valid_dma_address = (uint64_t)4 * (uint64_t)GB;
    //} else {
    //    max_valid_dma_address = ((uint64_t) maxdmaaddr) * MB;
        
    //    if ((max_valid_dma_address / PAGE_SIZE) < max_valid_low_ppnum)
    //        max_valid_low_ppnum = (ppnum_t)(max_valid_dma_address / PAGE_SIZE);
    //}
    if (avail_end >= max_valid_dma_address) {
        
        //if (!PE_parse_boot_argn("maxloreserve", &maxloreserve, sizeof (maxloreserve))) {
            
            if (sane_size >= (ONEGIG * 15))
                maxloreserve = (MAXLORESERVE / PAGE_SIZE) * 4;
            else if (sane_size >= (ONEGIG * 7))
                maxloreserve = (MAXLORESERVE / PAGE_SIZE) * 2;
            else
                maxloreserve = MAXLORESERVE / PAGE_SIZE;
            
        //} else
        //    maxloreserve = (maxloreserve * (1024 * 1024)) / PAGE_SIZE;
        
        if (maxloreserve) {
            vm_lopage_free_limit = maxloreserve;
            
            if (mbuf_override == true) {
                vm_lopage_free_limit += mbuf_reserve;
                vm_lopage_lowater = 0;
            } else
                vm_lopage_lowater = vm_lopage_free_limit / 16;
            
            vm_lopage_refill = true;
            vm_lopage_needed = true;
        }
    }
    
    /*
     *	Initialize kernel physical map.
     *	Kernel virtual address starts at VM_KERNEL_MIN_ADDRESS.
     */
    printf("avail_remaining = 0x%lx\n", (unsigned long)avail_remaining);
    pmap_bootstrap(0, IA32e);
}
