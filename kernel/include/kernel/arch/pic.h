//
//  pic.h
//  OS
//
//  Created by Adam Kopeć on 12/16/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#ifndef pic_h
#define pic_h

#include <stdint.h>
#include <sys/cdefs.h>

void PIC_sendEOI(unsigned char irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif /* pic_h */
