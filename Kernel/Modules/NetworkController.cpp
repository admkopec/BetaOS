//
//  NetworkController.cpp
//  Kernel
//
//  Created by Adam Kopeć on 8/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "NetworkController.hpp"

#define super Controller

OSReturn
NetworkController::sendPacket(__unused const void* data, __unused uint16_t length) {
    return kOSReturnFailed;
}

void
NetworkController::stop() {
    super::stop();
}

void
NetworkController::start() {
    super::start();
}
