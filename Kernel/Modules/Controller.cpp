//
//  Controller.cpp
//  Kernel
//
//  Created by Adam Kopeć on 3/6/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include "Controller.hpp"

void Controller::start() {
 // Default start routines
}

bool Controller::Used() {
    return Used_;
}

char* Controller::Name() {
    return NameString;
}

void Controller::stop() {
    // Default stop routines
    this->release();
}

void Controller::handleInterrupt() {
    // Default handleInterrupt routines
}

OSReturn Controller::init(PCI *header __unused) {
    return kOSReturnError;
}

Controller::~Controller() {
    
}
