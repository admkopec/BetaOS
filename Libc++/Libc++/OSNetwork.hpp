//
//  OSNetwork.hpp
//  Libc++
//
//  Created by Adam Kopeć on 8/17/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef OSNetwork_hpp
#define OSNetwork_hpp

#include "OSObject.hpp"
#include <Modules/NetworkController.hpp>

#define kMaxNetworkControllers 25

struct NetworkControllerTable {
    bool isRegistered;
    NetworkController* controller;
};

class OSNetwork : public OSObject {
    static NetworkControllerTable controllers[kMaxNetworkControllers];
    
public:
    static void registerController(NetworkController* controller);
    static NetworkController* getController(void);
};

#endif /* OSNetwork_hpp */
