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
extern "C" {
#endif
    void ModulesStartController();
    
#ifdef __cplusplus
}
#define MAX_LOADED_MODULES 1024
class Modules {
public:
    void start(void);
};


#endif /* __cplusplus */
#endif /* ModulesController_hpp */
