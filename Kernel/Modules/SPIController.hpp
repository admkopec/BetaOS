//
//  SPIController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 8/31/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef SPIController_hpp
#define SPIController_hpp

#include <stdio.h>
#include <stdint.h>
#include "Controller.hpp"

class SPI : public Controller {
    static const int VendorIDs[1];
    static const int DeviceIDs[3];
    
public:
    virtual OSReturn init(PCI *header) override;
    virtual void     start() override;
};

#endif /* SPIController_hpp */
