//
//  InterruptController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 6/6/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef InterruptController_hpp
#define InterruptController_hpp

#include <stdio.h>
#include "Controller.hpp"

extern "C" { void IncommingInterrupt(int InterruptNumber); }

struct TableEntry {
    bool isSet;
    Controller* handler;
};

class Interrupt {
    static TableEntry Table[20];
public:
    static OSReturn Register (int InterruptLine, Controller* handler);
    static OSReturn Disable  (int InterruptLine);
    static OSReturn Incomming(int InterruptLine);
};

#endif /* InterruptController_hpp */
