//
//  Controller.hpp
//  Kernel
//
//  Created by Adam Kopeć on 3/6/17.
//  Copyright © 2017 Adam Kopeć. All rights reserved.
//

#ifndef Controller_hpp
#define Controller_hpp

#include <OSObject.hpp>
#include "PCIController.hpp"
#include "MMIOUtils.hpp"

class Controller : public OSObject {
protected:
    bool Used_;
public:
    virtual int  init(PCI *header);
    virtual void start(void);
    virtual void stop(void);
    bool         Used(void);
    virtual     ~Controller();
};

#endif /* Controller_hpp */
