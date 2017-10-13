//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#include <time.h>
#include <i386/pio.h>
#include <kernel/command.h>
#include <platform/platform.h>
#include <platform/boot.h>
#include <i386/cpuid.h>
#include <i386/cpu_data.h>
#include <version.h>
#include <i386/rtclock.h>

#ifdef DEBUG
#define BUILD_TYPE "Internal"
#else
#define BUILD_TYPE ""
#endif

static inline void hlt() {
    pal_hlt();
}

static inline void sti() {
    pal_sti();
}

static inline void cli() {
    pal_cli();
}

extern bool VendorisApple;
extern void ModulesStartController(void);
extern void PrintLoadedModules(void);
extern int  putchar(int);
static inline unsigned int
GetBaseAddr(unsigned char Bus, unsigned char Slot, unsigned char Function) {
    return (unsigned int)(Bus) << 16 | 0x80000000 | (unsigned int)(Slot) << 11 | (unsigned int)(Function) << 8;
}
extern void
change_color(unsigned int foreground, unsigned int background);
