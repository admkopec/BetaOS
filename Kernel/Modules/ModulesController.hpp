//
//  ModulesController.hpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef ModulesController_hpp
#define ModulesController_hpp

//#include <stdio.h>
#include <kernel/misc_protos.h>
#ifdef __cplusplus
#include "PCIController.hpp"
#include "Controller.hpp"
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif
    void ModulesStartController(void);
    void ModulesStopController(void);
    void PrintLoadedModules(void);
    void SearchForEntrySMBios(void);
    
#ifdef __cplusplus
}
#define MAX_LOADED_MODULES 25
class Modules {
    uint32_t    LastLoadedModule = 0;
    Controller* Controllers[MAX_LOADED_MODULES];
public:
    void start(void);
    void stop(void);
    void print(void);
};

#endif /* __cplusplus */
#endif /* ModulesController_hpp */
