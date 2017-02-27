//
//  SATAController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 2/25/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef SATAController_hpp
#define SATAController_hpp

#include <stdio.h>
#include <stdint.h>
#include "PCIController.hpp"

class SATA {
    //uint64_t address;
    
public:
    int  init(PCI *header);
    void start();
};

#endif /* SATAController_hpp */
