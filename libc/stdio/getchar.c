//
//  getchar.c
//  BetaOS
//
//  Created by Adam Kopeć on 2/9/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <PS2Controller.h>
#include <kernel/interrupts.h>

int getchar() {
    return pollchar();
}