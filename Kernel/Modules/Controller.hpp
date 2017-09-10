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
    bool  Used_;
    char* NameString = (char*)"Generic Controller";
public:
    virtual OSReturn  init(PCI *header);
    virtual void      start(void);
    virtual void      stop(void);
    virtual void      handleInterrupt(void);
    bool              Used(void);
    char*             Name(void);
    virtual          ~Controller();
};

#endif /* Controller_hpp */
