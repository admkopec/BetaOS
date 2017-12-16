//
//  OSNetwork.cpp
//  Libc++
//
//  Created by Adam Kopeć on 8/17/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#include <Modules/NetworkController.hpp>
#include <OSNetwork.hpp>
#include "DHCP.hpp"

void
OSNetwork::registerController(NetworkController* controller) {
    if (controller == NULL) {
        return;
    }
    for (int i = 0; i < kMaxNetworkControllers; i++) {
        if (!controllers[i].isRegistered) {
            controllers[i].controller = controller;
            controllers[i].isRegistered = true;
            DHCP::Discover();
            return;
        }
    }
}

NetworkController *
OSNetwork::getController() {
    for (int i = 0; i < kMaxNetworkControllers; i++) {
        if (controllers[i].isRegistered) {
            return controllers[i].controller;
        }
    }
    return NULL;
}

NetworkControllerTable OSNetwork::controllers[kMaxNetworkControllers];
