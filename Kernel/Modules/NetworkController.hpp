//
//  NetworkController.hpp
//  Kernel
//
//  Created by Adam Kopeć on 8/16/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef NetworkController_hpp
#define NetworkController_hpp

#include <stdio.h>
#include "Controller.hpp"
#include "../../Libc++/Libc++/OSNetwork/IPs.hpp"

class NetworkController : public Controller {
public:
    uint8_t MAC[6];
    IP4_t   IP;
    IP4_t   Subnet;
    IP4_t   Gateway_IP;
    virtual OSReturn sendPacket(const void* data, uint16_t length);
    virtual void start();
    virtual void stop();
};

#endif /* NetworkController_hpp */
