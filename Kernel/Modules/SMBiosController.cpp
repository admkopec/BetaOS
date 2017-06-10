//
//  SMBiosController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 2/12/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SMBiosController.hpp"

extern "C" void SearchForEntrySMBios(void);

uint16_t SMBiosController::SMBiosTableLength;

int
SMBiosController::init() {
    SMBiosTable         = NULL;
    SMBiosTableLength   = 0;
    // Get SMBios Location
    return -2;
}

void SearchForEntrySMBios(void) {
    uint64_t Address;
    for (Address = 0x000F0000; Address < 0x000FFFFF; Address += 0x10) {
        if (!(strncmp((const char*)((uint64_t *)((Address))), "_SM_", 4))) {
            printf("Found SMBios Entry Point!\n");
            SMBEntryPoint* Entry = (SMBEntryPoint *)Address;
            SMBiosController::SMBiosTableLength = Entry->dmi.tableLength;
            /*for (SMBStructHeader* i = (SMBStructHeader*)Entry->dmi.tableAddress; (uint64_t)i < Entry->dmi.tableAddress+Entry->dmi.tableLength; i+=i->length) {
                if (i->type == 0) {
                    SMBBIOSInformation BiosInfo = *((SMBBIOSInformation*)(i));
                    printf("Vendor: %c %d\nVersion %d\n", BiosInfo.vendor, BiosInfo.vendor, BiosInfo.version);
                } else {
                    printf("Type: %d\n", i->type);
                }
            }*/
        }
    }
}
