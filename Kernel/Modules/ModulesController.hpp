//
//  ModulesController.hpp
//  BetaOS
//
//  Created by Adam Kopeć on 7/20/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef ModulesController_hpp
#define ModulesController_hpp

//#include <stdio.h>
#include <kernel/misc_protos.h>

#ifdef __cplusplus
extern "C" {
#endif
    void ModulesStartController();
    
#ifdef __cplusplus
}
#define MAX_LOADED_MODULES 1024
class Modules {
    int   num_of_loaded_modules;
    char* loaded_modules[MAX_LOADED_MODULES];
    
public:
    static void LoadModule();
};


#endif /* __cplusplus */
#endif /* ModulesController_hpp */
