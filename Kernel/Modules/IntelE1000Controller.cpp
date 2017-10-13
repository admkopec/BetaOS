//
//  IntelE1000Controller.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

// To be implemented properly

#include "IntelE1000Controller.hpp"
#include "InterruptController.hpp"
#include "MMIOUtils.hpp"
#include <stdint.h>
#include <i386/pio.h>
#include <OSNetwork.hpp>

#define super NetworkController
#define Log(x ...) printf("IntelE1000Controller: " x)
#ifdef DEBUG
#define DBG(x ...) Log(x)
#else
#define DBG(x ...)
#endif

OSReturn
E1000::init(PCI * pciConfigHeader) {
    for (size_t i = 0; i < sizeof(SupportedVendorIDs); i++) {
        if (pciConfigHeader->VendorID() == SupportedVendorIDs[i]) {
            break;
        } else if (i == (sizeof(SupportedVendorIDs) - 1)) {
            return kOSReturnFailed;
        }
    }
    
    for (size_t i = 0; i < sizeof(SupportedDeviceIDs); i++) {
        if (pciConfigHeader->DeviceID() == SupportedDeviceIDs[i]) {
            break;
        } else if (i == (sizeof(SupportedDeviceIDs) - 1)) {
            return kOSReturnFailed;
        }
    }
    
    // Enable bus mastering
    pciConfigHeader->EnableBusMastering();
    eerprom_exists = false;
    bar_type = pciConfigHeader->getBAR(0);
    Log("BAR type    %X\n", bar_type);
    if (bar_type == 0x1) {
        io_base  = pciConfigHeader->BAR().u.port;
        Log("BAR port    %X\n", io_base);
    } else {
        mem_base = (uintptr_t)pciConfigHeader->BAR().u.address;
        Log("BAR address %X\n", mem_base);
    }
    intline = pciConfigHeader->IntLine();
    
    IP.iIP4 = 0;
    
    NameString = (char*)"IntelE1000Controller";
    Used_ = true;
    return kOSReturnSuccess;
}

void E1000::start() {
    detectEEProm();
    if (!readMACAddress())
        return /*false*/;
    //printMac();
    //startLink();
    Log("IRQ = %d\n", intline);
    for(int i = 0; i < 0x80; i++)
        writeCommand(0x5200 + i*4, 0);
    if (!Interrupt::Register(intline, (Controller *)this)) {
        enableInterrupt();
        rxinit();
        txinit();
        Log("Device is running!\n");
        OSNetwork::registerController((super *)this);
        super::start();
        return /*true*/;
    } else
        return /*false*/;
}

void E1000::stop() {
    Log("Stopping...\n");
    super::stop();
}

void E1000::rxinit() {
    void * ptr;
    struct e1000_rx_desc *descs;
    
    ptr = (void *)OSRuntime::OSMalloc((sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16)); // Should be a physical address
    
    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        rx_descs[i]->addr = (uint64_t)(uint8_t *)((8192 + 16));
        rx_descs[i]->status = 0;
    }
    
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
    
    writeCommand(REG_RXDESCLO, (uint32_t)((uint64_t)ptr));
    writeCommand(REG_RXDESCHI, 0);
    
    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
    
    writeCommand(REG_RXDESCHEAD, 0);
    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    rx_cur = 0;
    writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_2048);
    
}


void E1000::txinit() {
    void *  ptr;
    struct e1000_tx_desc *descs;
    
    ptr = (void *)OSRuntime::OSMalloc((sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16)); // Should be a physical address
    
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
    writeCommand(REG_TIPG,   0x0060200A);
    
}

void E1000::handleInterrupt() {
    uint32_t icr = readCommand(REG_ICAUSE);
    DBG("Interrupt: %x\n", icr);
    if (icr & ICR_RECEIVE) {
        handleReceive();
    } else if (icr & ICR_TRANSMIT) {
        DBG("Transmit\n");
    } else if (icr & ICR_LINK_CHANGE) {
        Log("Link %s\n", (readCommand(REG_STATUS) & STATUS_LINK_UP) ? "up" : "down");
    }
}

void E1000::handleReceive() {
    bool got_packet = false;
    
    uint32_t head = readCommand(REG_RXDESCHEAD);
    
    DBG("Handle Receive\n");
    
    while(rx_cur != head) {
        got_packet = true;
#ifdef DEBUG
        uint8_t *buf    = (uint8_t *)rx_descs[rx_cur]->addr;
#endif
        size_t   len    = rx_descs[rx_cur]->length;
        uint8_t  status = rx_descs[rx_cur]->status;
        
        if ((status & 0x1) == 0) {
            break;
        }
        
        len -= 4;
        
        DBG("Packet: %d Bytes received (status = %x)\n", len, status);
        
        DBG("Packet: %s length: %d\n", buf, len);
        // Here inject the received packet into the network stack
        
        rx_cur++;
        rx_cur %= E1000_NUM_RX_DESC;
        /*rx_descs[rx_cur]->status = 0;
        writeCommand(REG_RXDESCTAIL, old_cur );*/
    }
    if (rx_cur == head) {
        writeCommand(REG_RXDESCTAIL, (head + E1000_NUM_RX_DESC - 1) % E1000_NUM_RX_DESC);
    } else {
        writeCommand(REG_RXDESCTAIL, rx_cur);
    }
}

OSReturn E1000::sendPacket(const void * p_data, uint16_t p_len) {
    uint32_t old_cur = tx_cur;
    tx_cur++;
    tx_cur %= E1000_NUM_TX_DESC;
    uint32_t head = readCommand(REG_TXDESCHEAD);
    if (tx_cur == head) {
        Log("No place in Send Queue!\n");
        tx_cur = old_cur;
        return kOSReturnFailed;
    }
    
    if (p_len > E1000_SIZE_TX_DESC) {
        p_len = E1000_SIZE_TX_DESC;
    }
    
    tx_descs[tx_cur]->addr = (uint64_t)p_data;
    tx_descs[tx_cur]->length = p_len;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
    
    writeCommand(REG_RXDESCTAIL, tx_cur);
    /*tx_descs[tx_cur]->status = 0;
    uint8_t old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    writeCommand(REG_TXDESCTAIL, tx_cur);
    while(!(tx_descs[old_cur]->status & 0xff));*/
    DBG("Send Packet\n");
    return kOSReturnSuccess;
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
    
    for(int i = 0; i < 1000 && !eerprom_exists; i++) {
        val = readCommand(REG_EEPROM);
        if(val & 0x10)
            eerprom_exists = true;
        else
            eerprom_exists = false;
    }
    return eerprom_exists;
}

bool E1000::readMACAddress() {
    if (eerprom_exists) {
        uint32_t temp;
        temp = eepromRead(0);
        MAC[0] = temp &0xff;
        MAC[1] = temp >> 8;
        temp = eepromRead(1);
        MAC[2] = temp &0xff;
        MAC[3] = temp >> 8;
        temp = eepromRead(2);
        MAC[4] = temp &0xff;
        MAC[5] = temp >> 8;
    } else {
        uint8_t * mem_base_mac_8 = (uint8_t *) (mem_base+0x5400);
        uint32_t * mem_base_mac_32 = (uint32_t *) (mem_base+0x5400);
        if (mem_base_mac_32[0] != 0 ) {
            for (int i = 0; i < 6; i++) {
                MAC[i] = mem_base_mac_8[i];
            }
        }
        else return false;
    }
    return true;
}

void E1000::enableInterrupt() {
    writeCommand(REG_IMASK ,0x1F6DC);
    writeCommand(REG_IMASK ,0xFF & ~4);
    readCommand(0xC0);
    
}

uint32_t E1000::eepromRead( uint8_t addr) {
    uint16_t data = 0;
    uint32_t tmp = 0;
    if ( eerprom_exists) {
        writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 4)) );
    } else {
        writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 2) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 1)) );
    }
    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}

const int
E1000::SupportedVendorIDs[] = { 0x8086 };
const int
E1000::SupportedDeviceIDs[] = { 0x100E, 0x153A, 0x10D3, 0x10EA, 0x1502, 0x1503, 0x1000,
                                0x1001, 0x1004, 0x1008, 0x1009, 0x100C, 0x100D, 0x1015,
                                0x1016, 0x1017, 0x101E, 0x100F, 0x1011, 0x1026, 0x1027,
                                0x1028, 0x1010, 0x1012, 0x101D, 0x1013, 0x1018, 0x1014,
                                0x1078, 0x1075, 0x1076, 0x1077, 0x107C, 0x1079, 0x107A,
                                0x107B, 0x108A, 0x1099, 0x1019, 0x101A, 0x10B5, 0x2E6E };
