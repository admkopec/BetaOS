//
//  pal.c
//  BetaOS
//
//  Created by Adam Kopeć on 6/29/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#include <i386/pal.h>
#include <i386/proc_reg.h>

/* nanotime convertion info */
pal_rtc_nanotime_t pal_rtc_nanotime_info = {0,0,0,0,1,0};

void
pal_get_control_registers(pal_cr_t *cr0, pal_cr_t *cr2, pal_cr_t *cr3, pal_cr_t *cr4) {
    *cr0 = get_cr0();
    *cr2 = get_cr2();
    *cr3 = get_cr3_raw();
    *cr4 = get_cr4();
}


/* <Temporary> */
#include <i386/pio.h>
/* standard port addresses */
enum {
    COM1_PORT_ADDR = 0x3f8,
    COM2_PORT_ADDR = 0x2f8
};

/* UART register offsets */
enum {
    UART_RBR = 0,  /* receive buffer Register   (R) */
    UART_THR = 0,  /* transmit holding register (W) */
    UART_DLL = 0,  /* DLAB = 1, divisor latch (LSB) */
    UART_IER = 1,  /* interrupt enable register     */
    UART_DLM = 1,  /* DLAB = 1, divisor latch (MSB) */
    UART_IIR = 2,  /* interrupt ident register (R)  */
    UART_FCR = 2,  /* fifo control register (W)     */
    UART_LCR = 3,  /* line control register         */
    UART_MCR = 4,  /* modem control register        */
    UART_LSR = 5,  /* line status register          */
    UART_MSR = 6,  /* modem status register         */
    UART_SCR = 7   /* scratch register              */
};

enum {
    UART_LCR_8BITS = 0x03,
    UART_LCR_DLAB  = 0x80
};

enum {
    UART_MCR_DTR   = 0x01,
    UART_MCR_RTS   = 0x02,
    UART_MCR_OUT1  = 0x04,
    UART_MCR_OUT2  = 0x08,
    UART_MCR_LOOP  = 0x10
};

enum {
    UART_LSR_DR    = 0x01,
    UART_LSR_OE    = 0x02,
    UART_LSR_PE    = 0x04,
    UART_LSR_FE    = 0x08,
    UART_LSR_THRE  = 0x20
};

static unsigned uart_baud_rate = 115200;
#define UART_PORT_ADDR  COM1_PORT_ADDR

#define UART_CLOCK  1843200   /* 1.8432 MHz clock */

#define WRITE(r, v)  outb(UART_PORT_ADDR + UART_##r, v)
#define READ(r)      inb(UART_PORT_ADDR + UART_##r)
#define DELAY(x)     { volatile int _d_; for (_d_ = 0; _d_ < (10000*x); _d_++) ; }

static int uart_initted = 0;   /* 1 if init'ed */

static int
uart_probe(void) {
    /* Verify that the Scratch Register is accessible */
    
    WRITE( SCR, 0x5a );
    if (READ(SCR) != 0x5a) return 0;
    WRITE( SCR, 0xa5 );
    if (READ(SCR) != 0xa5) return 0;
    return 1;
}

static void
uart_set_baud_rate(unsigned long baud_rate) {
    const unsigned char lcr = READ( LCR );
    unsigned long       div;
    
    if (baud_rate == 0) baud_rate = 9600;
    div = UART_CLOCK / 16 / baud_rate;
    WRITE( LCR, lcr | UART_LCR_DLAB );
    WRITE( DLM, (unsigned char)(div >> 8) );
    WRITE( DLL, (unsigned char) div );
    WRITE( LCR, lcr & ~UART_LCR_DLAB);
}

static void
uart_putc(char c) {
    if (!uart_initted) return;
    
    /* Wait for THR empty */
    while ( !(READ(LSR) & UART_LSR_THRE) ) DELAY(1);
    
    WRITE( THR, c );
}

static int
uart_getc(void) {
    /*
     * This function returns:
     * -1 : no data
     * -2 : receiver error
     * >0 : character received
     */
    
    unsigned char lsr;
    
    if (!uart_initted) return -1;
    
    lsr = READ(LSR);
    
    if (lsr & (UART_LSR_FE | UART_LSR_PE | UART_LSR_OE)) {
        READ( RBR ); /* discard */
        return -2;
    }
    
    if (lsr & UART_LSR_DR) {
        return READ(RBR);
    }
    
    return -1;
}

int
serial_init(void) {
    if (uart_probe() == 0) return 0;
    
    /* Disable hardware interrupts */
    
    WRITE(MCR, 0);
    WRITE(IER, 0);
    
    /* Disable FIFO's for 16550 devices */
    
    WRITE(FCR, 0);
    
    /* Set for 8-bit, no parity, DLAB bit cleared */
    
    WRITE(LCR, UART_LCR_8BITS);
    
    /* Set baud rate */
    
    uart_set_baud_rate( uart_baud_rate);
    
    /* Assert DTR# and RTS# lines (OUT2?) */
    
    WRITE(MCR, UART_MCR_DTR | UART_MCR_RTS);
    
    /* Clear any garbage in the input buffer */
    
    READ(RBR);
    
    uart_initted = 1;
    
    return 1;
}

void
serial_putc(int c) {
    uart_putc((char) c);
}

int
serial_getc(void) {
    return uart_getc();
}
/* </Temporary> */
