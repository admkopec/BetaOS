//
//  Time.hpp
//  OS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#pragma once
#ifndef Time_hpp
#define Time_hpp

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "interrupts.hpp"

class Time {            //Doesn't work
public:
    int weekday();
    unsigned int* date();
    unsigned int* time();
};

#endif /* Time_hpp */
