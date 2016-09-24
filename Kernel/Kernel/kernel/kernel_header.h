//
//  kernel_header.h
//  BetaOS
//
//  Created by Adam Kopeć on 7/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef kernel_header_h
#define kernel_header_h

#include <mach-o/loader.h>

#if	!KERNEL
#error this header for kernel use only
#endif

#ifdef __LP64__

typedef struct mach_header_64       kernel_mach_header_t;
typedef struct segment_command_64   kernel_segment_command_t;
typedef struct section_64           kernel_section_t;
typedef struct nlist_64             kernel_nlist_t;

#define MH_MAGIC_KERNEL         MH_MAGIC_64
#define LC_SEGMENT_KERNEL       LC_SEGMENT_64

#else

typedef struct mach_header		kernel_mach_header_t;
typedef struct segment_command	kernel_segment_command_t;
typedef struct section			kernel_section_t;
typedef struct nlist            kernel_nlist_t;

#define MH_MAGIC_KERNEL         MH_MAGIC
#define LC_SEGMENT_KERNEL		LC_SEGMENT
#define SECT_CONSTRUCTOR		"__constructor"
#define SECT_DESTRUCTOR			"__destructor"

#endif

#define SECT_MODINITFUNC		"__mod_init_func"
#define SECT_MODTERMFUNC		"__mod_term_func"

extern kernel_mach_header_t _mh_execute_header;

vm_offset_t getlastaddr(void);

kernel_segment_command_t *firstseg(void);
kernel_segment_command_t *firstsegfromheader(kernel_mach_header_t     *header);
kernel_segment_command_t *nextsegfromheader( kernel_mach_header_t	  *header, kernel_segment_command_t *seg);
kernel_segment_command_t *getsegbyname(const char *seg_name);
kernel_segment_command_t *getsegbynamefromheader(kernel_mach_header_t *header, const char *seg_name);
void *getsegdatafromheader(kernel_mach_header_t*, const char*, unsigned long*);
kernel_section_t *getsectbyname(const char *seg_name, const char *sect_name);
kernel_section_t *getsectbynamefromheader(kernel_mach_header_t *header,const char *seg_name, const char *sect_name);
void *getsectdatafromheader(kernel_mach_header_t *, const char *, const char *, unsigned long *);
kernel_section_t *firstsect(kernel_segment_command_t *sgp);
kernel_section_t *nextsect(kernel_segment_command_t *sgp, kernel_section_t *sp);
void *getcommandfromheader(kernel_mach_header_t *, uint32_t);
void *getuuidfromheader(kernel_mach_header_t *, unsigned long *);


#endif /* kernel_header_h */
