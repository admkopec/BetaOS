//
//  IntelE1000Controller.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "IntelE1000Controller.hpp"
#include "MMIOUtils.hpp"
#include <stdint.h>
#include <i386/pio.h>

E1000::E1000(/*PCIConfigHeader * _pciConfigHeader*/) {
    // Get BAR0 type, io_base address and MMIO base address
    //bar_type = pciConfigHeader->getPCIBarType(0);
    //io_base = pciConfigHeader->getPCIBar(PCI_BAR_IO) & ~1;
    //mem_base = pciConfigHeader->getPCIBar( PCI_BAR_MEM) & ~3;
    
    
    // Enable bus mastering
    //pciConfigHeader->enablePCIBusMastering();
    eerprom_exists = false;
    bar_type = 0;
    mem_base = 0xF0000000;
}

E1000::~E1000() { }

bool E1000::start() {
    detectEEProm();
    if (!readMACAddress()) return false;
    //printMac();
    //startLink();
    
    for(int i = 0; i < 0x80; i++)
        writeCommand(0x5200 + i*4, 0);
    if (/*interruptManager->registerInterrupt(IRQ0+pciConfigHeader->getIntLine(),this)*/1)
    {
        //enableInterrupt();
        rxinit();
        txinit();
        //video.putString("E1000 card started\n",COLOR_RED,COLOR_WHITE);
        return true;
    }
    else return false;
}

void E1000::rxinit() {
    uint8_t * ptr;
    struct e1000_rx_desc *descs;
    
    ptr = (uint8_t *)((sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16)); // Should be a physical address
    
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16); // Should be a physical address
        rx_descs[i]->addr = (uint64_t)(uint8_t *)((8192 + 16));
        rx_descs[i]->status = 0;
    }
    
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
    
    writeCommand(REG_RXDESCLO, (uint64_t)ptr);
    writeCommand(REG_RXDESCHI, 0);
    
    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
    
    writeCommand(REG_RXDESCHEAD, 0);
    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    rx_cur = 0;
    writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_2048);
    
}


void E1000::txinit() {
    uint8_t *  ptr;
    struct e1000_tx_desc *descs;
    
    ptr = (uint8_t *)((sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16)); // Should be a physical address
    
    descs = (struct e1000_tx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++) {
        tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
        tx_descs[i]->addr = 0;
        tx_descs[i]->cmd = 0;
        tx_descs[i]->status = TSTA_DD;
    }
    
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
    
    
    //now setup total length of descriptors
    writeCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
    
    
    //setup numbers
    writeCommand( REG_TXDESCHEAD, 0);
    writeCommand( REG_TXDESCTAIL, 0);
    tx_cur = 0;
    writeCommand(REG_TCTRL,  TCTL_EN
                 | TCTL_PSP
                 | (15 << TCTL_CT_SHIFT)
                 | (64 << TCTL_COLD_SHIFT)
                 | TCTL_RTLC);
    
    // For E1000e
    // (Overwrites the line above)
    writeCommand(REG_TCTRL,  0b0110000000000111111000011111010);
    writeCommand(REG_TIPG,  0x0060200A);
    
}

void E1000::handleReceive() {
    uint16_t old_cur;
    bool got_packet = false;
    
    while((rx_descs[rx_cur]->status & 0x1)) {
        got_packet = true;
        __unused uint8_t *buf = (uint8_t *)rx_descs[rx_cur]->addr;
        __unused uint16_t len = rx_descs[rx_cur]->length;
        
        // Here inject the received packet into the network stack
        
        rx_descs[rx_cur]->status = 0;
        old_cur = rx_cur;
        rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
        writeCommand(REG_RXDESCTAIL, old_cur );
    }
}

int E1000::sendPacket(const void * p_data, uint16_t p_len) {
    tx_descs[tx_cur]->addr = (uint64_t)p_data;
    tx_descs[tx_cur]->length = p_len;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
    tx_descs[tx_cur]->status = 0;
    uint8_t old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    writeCommand(REG_TXDESCTAIL, tx_cur);
    while(!(tx_descs[old_cur]->status & 0xff));
    return 0;
}

void E1000::writeCommand(uint16_t p_address, uint32_t p_value) {
    if (bar_type == 0) {
        MMIOUtils::write32(mem_base+p_address,p_value);
    } else {
        outl(io_base, p_address);
        outl(io_base + 4, p_value);
    }
}
uint32_t E1000::readCommand(uint16_t p_address) {
    if (bar_type == 0) {
        return MMIOUtils::read32(mem_base+p_address);
    } else {
        outl(io_base, p_address);
        return inl(io_base + 4);
    }
}

bool E1000::detectEEProm() {
    uint32_t val = 0;
    writeCommand(REG_EEPROM, 0x1);
    
    for(int i = 0; i < 1000 && !eerprom_exists; i++)
    {
        val = readCommand(REG_EEPROM);
        if(val & 0x10)
            eerprom_exists = true;
        else
            eerprom_exists = false;
    }
    return eerprom_exists;
}

bool E1000::readMACAddress() {
    if (eerprom_exists)
    {
        uint32_t temp;
        temp = eepromRead(0);
        mac[0] = temp &0xff;
        mac[1] = temp >> 8;
        temp = eepromRead(1);
        mac[2] = temp &0xff;
        mac[3] = temp >> 8;
        temp = eepromRead(2);
        mac[4] = temp &0xff;
        mac[5] = temp >> 8;
    }
    else
    {
        uint8_t * mem_base_mac_8 = (uint8_t *) (mem_base+0x5400);
        uint32_t * mem_base_mac_32 = (uint32_t *) (mem_base+0x5400);
        if ( mem_base_mac_32[0] != 0 )
        {
            for(int i = 0; i < 6; i++)
            {
                mac[i] = mem_base_mac_8[i];
            }
        }
        else return false;
    }
    return true;
}

uint8_t * E1000::getMacAddress() {
    readMACAddress();
    return mac;
}

uint32_t E1000::eepromRead( uint8_t addr) {
    uint16_t data = 0;
    uint32_t tmp = 0;
    if ( eerprom_exists) {
        writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 4)) );
    }
    else {
        writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 2) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 1)) );
    }
    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}
