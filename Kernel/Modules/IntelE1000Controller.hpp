//
//  IntelE1000Controller.hpp
//  BetaOS
//
//  Created by Adam Kopeć on 6/20/16.
//  Copyright © 2016-2017 Adam Kopeć. All rights reserved.
//

#ifndef IntelE1000Controller_hpp
#define IntelE1000Controller_hpp

#include <stdio.h>
#include "NetworkController.hpp"

#define Intel_Vendor                0x8086  // Vendor ID for Intel
#define E1000_82540EM               0x100E  // Device ID for the e1000 Qemu, Bochs, and VirtualBox emmulated NICs
#define E1000_I217                  0x153A  // Device ID for Intel I217
#define E1000_82574                 0x10D3  // Device ID for Intel 82574 (not sure if it'll work)
#define E1000_82577LM               0x10EA  // Device ID for Intel 82577LM
#define E1000_82579LM               0x1502  // Device ID for Intel 82579LM
#define E1000_82579V                0x1503  // Device ID for Intel 82579V
#define E1000_82542                 0x1000
#define E1000_82543GC_FIBER         0x1001
#define E1000_82543GC_COPPER        0x1004
#define E1000_82544EI_COPPER        0x1008
#define E1000_82544EI_FIBER         0x1009
#define E1000_82544GC_COPPER        0x100C
#define E1000_82544GC_LOM           0x100D
#define E1000_82540EM_LOM           0x1015
#define E1000_82540EP_LOM           0x1016
#define E1000_82540EP               0x1017
#define E1000_82540EP_LP            0x101E
#define E1000_82545EM_COPPER        0x100F
#define E1000_82545EM_FIBER         0x1011
#define E1000_82545GM_COPPER        0x1026
#define E1000_82545GM_FIBER         0x1027
#define E1000_82545GM_SERDES        0x1028
#define E1000_82546EB_COPPER        0x1010
#define E1000_82546EB_FIBER         0x1012
#define E1000_82546EB_QUAD_COPPER   0x101D
#define E1000_82541EI               0x1013
#define E1000_82541EI_MOBILE        0x1018
#define E1000_82541ER_LOM           0x1014
#define E1000_82541ER               0x1078
#define E1000_82547GI               0x1075
#define E1000_82541GI               0x1076
#define E1000_82541GI_MOBILE        0x1077
#define E1000_82541GI_LF            0x107C
#define E1000_82546GB_COPPER        0x1079
#define E1000_82546GB_FIBER         0x107A
#define E1000_82546GB_SERDES        0x107B
#define E1000_82546GB_PCIE          0x108A
#define E1000_82546GB_QUAD_COPPER   0x1099
#define E1000_82547EI               0x1019
#define E1000_82547EI_MOBILE        0x101A
#define E1000_82546GB_QUAD_COPPER_2 0x10B5
#define E1000_INTEL_CE4100_GBE      0x2E6E

#define REG_CTRL        0x0000
#define REG_STATUS      0x0008
#define REG_EEPROM      0x0014
#define REG_CTRL_EXT    0x0018
#define REG_ICAUSE      0x00C0
#define REG_IMASK       0x00D0
#define REG_IMASKCLR    0x00D8
#define REG_RCTRL       0x0100
#define REG_RXDESCLO    0x2800
#define REG_RXDESCHI    0x2804
#define REG_RXDESCLEN   0x2808
#define REG_RXDESCHEAD  0x2810
#define REG_RXDESCTAIL  0x2818

#define REG_TCTRL       0x0400
#define REG_TXDESCLO    0x3800
#define REG_TXDESCHI    0x3804
#define REG_TXDESCLEN   0x3808
#define REG_TXDESCHEAD  0x3810
#define REG_TXDESCTAIL  0x3818


#define REG_RDTR         0x2820 // RX Delay Timer Register
#define REG_RXDCTL       0x3828 // RX Descriptor Control
#define REG_RADV         0x282C // RX Int. Absolute Delay Timer
#define REG_RSRPD        0x2C00 // RX Small Packet Detect Interrupt



#define REG_TIPG         0x0410      // Transmit Inter Packet Gap
#define ECTRL_SLU        0x40        // Set link up


#define RCTL_EN                         (1 << 1)    // Receiver Enable
#define RCTL_SBP                        (1 << 2)    // Store Bad Packets
#define RCTL_UPE                        (1 << 3)    // Unicast Promiscuous Enabled
#define RCTL_MPE                        (1 << 4)    // Multicast Promiscuous Enabled
#define RCTL_LPE                        (1 << 5)    // Long Packet Reception Enable
#define RCTL_LBM_NONE                   (0 << 6)    // No Loopback
#define RCTL_LBM_PHY                    (3 << 6)    // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF                 (0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER              (1 << 8)    // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH               (2 << 8)    // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36                      (0 << 12)   // Multicast Offset - bits 47:36
#define RCTL_MO_35                      (1 << 12)   // Multicast Offset - bits 46:35
#define RCTL_MO_34                      (2 << 12)   // Multicast Offset - bits 45:34
#define RCTL_MO_32                      (3 << 12)   // Multicast Offset - bits 43:32
#define RCTL_BAM                        (1 << 15)   // Broadcast Accept Mode
#define RCTL_VFE                        (1 << 18)   // VLAN Filter Enable
#define RCTL_CFIEN                      (1 << 19)   // Canonical Form Indicator Enable
#define RCTL_CFI                        (1 << 20)   // Canonical Form Indicator Bit Value
#define RCTL_DPF                        (1 << 22)   // Discard Pause Frames
#define RCTL_PMCF                       (1 << 23)   // Pass MAC Control Frames
#define RCTL_SECRC                      (1 << 26)   // Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256                  (3 << 16)
#define RCTL_BSIZE_512                  (2 << 16)
#define RCTL_BSIZE_1024                 (1 << 16)
#define RCTL_BSIZE_2048                 (0 << 16)
#define RCTL_BSIZE_4096                 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192                 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384                ((1 << 16) | (1 << 25))


// Transmit Command

#define CMD_EOP                         (1 << 0)    // End of Packet
#define CMD_IFCS                        (1 << 1)    // Insert FCS
#define CMD_IC                          (1 << 2)    // Insert Checksum
#define CMD_RS                          (1 << 3)    // Report Status
#define CMD_RPS                         (1 << 4)    // Report Packet Sent
#define CMD_VLE                         (1 << 6)    // VLAN Packet Enable
#define CMD_IDE                         (1 << 7)    // Interrupt Delay Enable

// ICR Types

#define ICR_TRANSMIT                    (1 << 0)
#define ICR_LINK_CHANGE                 (1 << 2)
#define ICR_RECEIVE                     (1 << 7)

#define STATUS_LINK_UP                  (1 << 1)

// TCTL Register

#define TCTL_EN                         (1 << 1)    // Transmit Enable
#define TCTL_PSP                        (1 << 3)    // Pad Short Packets
#define TCTL_CT_SHIFT                   4           // Collision Threshold
#define TCTL_COLD_SHIFT                 12          // Collision Distance
#define TCTL_SWXOFF                     (1 << 22)   // Software XOFF Transmission
#define TCTL_RTLC                       (1 << 24)   // Re-transmit on Late Collision

#define TSTA_DD                         (1 << 0)    // Descriptor Done
#define TSTA_EC                         (1 << 1)    // Excess Collisions
#define TSTA_LC                         (1 << 2)    // Late Collision
#define LSTA_TU                         (1 << 3)    // Transmit Underrun

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8
#define E1000_SIZE_RX_DESC 2048
#define E1000_SIZE_TX_DESC 2048

struct e1000_rx_desc {
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint16_t checksum;
    volatile uint8_t status;
    volatile uint8_t errors;
    volatile uint16_t special;
} __attribute__((packed));

struct e1000_tx_desc {
    volatile uint64_t addr;
    volatile uint16_t length;
    volatile uint8_t cso;
    volatile uint8_t cmd;
    volatile uint8_t status;
    volatile uint8_t css;
    volatile uint16_t special;
} __attribute__((packed));

class E1000 : public NetworkController {
private:
    uint8_t     bar_type;       // Type of BOR0
    uint8_t     intline;        // Interrupt Line
    uint16_t    io_base;        // IO Base Address
    uint64_t    mem_base;       // MMIO Base Address
    bool        eerprom_exists; // A flag indicating if eeprom exists
    uint8_t     MAC[6];         // A buffer for storing the mack address
    struct      e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC]; // Receive Descriptor Buffers
    struct      e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC]; // Transmit Descriptor Buffers
    uint16_t    rx_cur;         // Current Receive Descriptor Buffer
    uint16_t    tx_cur;         // Current Transmit Descriptor Buffer
    
    static const int SupportedVendorIDs[1];
    static const int SupportedDeviceIDs[42];
    
    // Send Commands and read results From NICs either using MMIO or IO Ports
    void     writeCommand(uint16_t p_address, uint32_t p_value);
    uint32_t readCommand(uint16_t p_address);
    
    
    bool        detectEEProm();         // Return true if EEProm exist, else it returns false and set the eerprom_existsdata member
    uint32_t    eepromRead(uint8_t addr); // Read 4 bytes from a specific EEProm Address
    bool        readMACAddress();       // Read MAC Address
    void        startLink();            // Start up the network
    void        rxinit();               // Initialize receive descriptors an buffers
    void        txinit();               // Initialize transmit descriptors an buffers
    void        enableInterrupt();      // Enable Interrupts
    void        handleReceive();        // Handle a packet reception.
public:
    virtual int  init(PCI *h) override;
    virtual void start() override;                              // Perform initialization tasks and starts the driver
    virtual void stop()  override;                              // Perform stop routines
    virtual void handleInterrupt() override;                    // Handle an Interrupt
    virtual OSReturn sendPacket(const void* data, uint16_t length) override; // Send a packet
};

#endif /* IntelE1000Controller_hpp */
