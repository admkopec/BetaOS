//
//  SMBiosController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 2/12/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "SMBiosController.hpp"

int SMBiosController::init() {
    SMBiosTable         = NULL;
    SMBiosTableLength   = 0;
    // Get SMBios Location
    return -2;
}
