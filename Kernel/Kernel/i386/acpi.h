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

void acpi(void);
void acpipoweroff(void);
void acpireboot(void);

#endif /* acpi_h */
