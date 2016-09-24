//
//  sel.h
//  BetaOS
//
//  Created by Adam Kopeć on 4/25/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

#ifndef sel_h
#define sel_h

typedef struct sel {
    unsigned short	rpl     :2,
#define KERN_PRIV	0
#define USER_PRIV	3
                    ti      :1,
#define SEL_GDT		0
#define SEL_LDT		1
                    index	:13;
} sel_t;

#define NULL_SEL	((sel_t) { 0, 0, 0 } )

#endif /* sel_h */
