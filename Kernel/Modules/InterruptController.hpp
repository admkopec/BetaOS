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

class Interrupt {
public:
    static int RegisterInterrupt(int InterruptLine, void (*handler)());
};

#endif /* InterruptController_hpp */
