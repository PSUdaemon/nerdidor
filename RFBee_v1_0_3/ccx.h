#ifndef _CC1100_H
#define _CC1100_H
#include "Atmegax.h"

// CC2500/CC1100/CC1101 STROBE, CONTROL AND STATUS REGISTER
#define CCx_IOCFG2       0x00        // GDO2 output pin configuration
#define CCx_IOCFG1       0x01        // GDO1 output pin configuration
#define CCx_IOCFG0       0x02        // GDO0 output pin configuration
#define CCx_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CCx_SYNC1        0x04        // Sync word, high byte
#define CCx_SYNC0        0x05        // Sync word, low byte
#define CCx_PKTLEN       0x06        // Packet length
#define CCx_PKTCTRL1     0x07        // Packet automation control
#define CCx_PKTCTRL0     0x08        // Packet automation control
#define CCx_ADDR         0x09        // Device address
#define CCx_CHANNR       0x0A        // Channel number
#define CCx_FSCTRL1      0x0B        // Frequency synthesizer control
#define CCx_FSCTRL0      0x0C        // Frequency synthesizer control
#define CCx_FREQ2        0x0D        // Frequency control word, high byte
#define CCx_FREQ1        0x0E        // Frequency control word, middle byte
#define CCx_FREQ0        0x0F        // Frequency control word, low byte
#define CCx_MDMCFG4      0x10        // Modem configuration
#define CCx_MDMCFG3      0x11        // Modem configuration
#define CCx_MDMCFG2      0x12        // Modem configuration
#define CCx_MDMCFG1      0x13        // Modem configuration
#define CCx_MDMCFG0      0x14        // Modem configuration
#define CCx_DEVIATN      0x15        // Modem deviation setting
#define CCx_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CCx_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CCx_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CCx_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CCx_BSCFG        0x1A        // Bit Synchronization configuration
#define CCx_AGCCTRL2     0x1B        // AGC control
#define CCx_AGCCTRL1     0x1C        // AGC control
#define CCx_AGCCTRL0     0x1D        // AGC control
#define CCx_WOREVT1      0x1E        // High byte Event 0 timeout
#define CCx_WOREVT0      0x1F        // Low byte Event 0 timeout
#define CCx_WORCTRL      0x20        // Wake On Radio control
#define CCx_FREND1       0x21        // Front end RX configuration
#define CCx_FREND0       0x22        // Front end TX configuration
#define CCx_FSCAL3       0x23        // Frequency synthesizer calibration
#define CCx_FSCAL2       0x24        // Frequency synthesizer calibration
#define CCx_FSCAL1       0x25        // Frequency synthesizer calibration
#define CCx_FSCAL0       0x26        // Frequency synthesizer calibration
#define CCx_RCCTRL1      0x27        // RC oscillator configuration
#define CCx_RCCTRL0      0x28        // RC oscillator configuration
#define CCx_FSTEST       0x29        // Frequency synthesizer calibration control
#define CCx_PTEST        0x2A        // Production test
#define CCx_AGCTEST      0x2B        // AGC test
#define CCx_TEST2        0x2C        // Various test settings
#define CCx_TEST1        0x2D        // Various test settings
#define CCx_TEST0        0x2E        // Various test settings

// Strobe commands
#define CCx_SRES         0x30        // Reset chip.
#define CCx_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CCx_SXOFF        0x32        // Turn off crystal oscillator.
#define CCx_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CCx_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CCx_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CCx_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CCx_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CCx_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CCx_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CCx_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CCx_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CCx_SWORRST      0x3C        // Reset real time clock.
#define CCx_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // bytes for simpler software.
// Status registers (read & burst)
#define CCx_PARTNUM      (0x30 | 0xc0)
#define CCx_VERSION      (0x31 | 0xc0)
#define CCx_FREQEST      (0x32 | 0xc0)
#define CCx_LQI          (0x33 | 0xc0)
#define CCx_RSSI         (0x34 | 0xc0)
#define CCx_MARCSTATE    (0x35 | 0xc0)
#define CCx_WORTIME1     (0x36 | 0xc0)
#define CCx_WORTIME0     (0x37 | 0xc0)
#define CCx_PKTSTATUS    (0x38 | 0xc0)
#define CCx_VCO_VC_DAC   (0x39 | 0xc0)
#define CCx_TXBYTES      (0x3A | 0xc0)
#define CCx_RXBYTES      (0x3B | 0xc0)

#define CCx_PATABLE      0x3E
#define CCx_TXFIFO       0x3F
#define CCx_RXFIFO       0x3F

// RF_SETTINGS is a data structure which contains all relevant CCx registers
typedef struct RF_SETTINGS {
    unsigned char FSCTRL1;   // Frequency synthesizer control.
    unsigned char FSCTRL0;   // Frequency synthesizer control.
    unsigned char FREQ2;     // Frequency control word, high byte.
    unsigned char FREQ1;     // Frequency control word, middle byte.
    unsigned char FREQ0;     // Frequency control word, low byte.
    unsigned char MDMCFG4;   // Modem configuration.
    unsigned char MDMCFG3;   // Modem configuration.
    unsigned char MDMCFG2;   // Modem configuration.
    unsigned char MDMCFG1;   // Modem configuration.
    unsigned char MDMCFG0;   // Modem configuration.
    unsigned char CHANNR;    // Channel number.
    unsigned char DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).
    unsigned char FREND1;    // Front end RX configuration.
    unsigned char FREND0;    // Front end RX configuration.
    unsigned char MCSM0;     // Main Radio Control State Machine configuration.
    unsigned char FOCCFG;    // Frequency Offset Compensation Configuration.
    unsigned char BSCFG;     // Bit synchronization Configuration.
    unsigned char AGCCTRL2;  // AGC control.
	unsigned char AGCCTRL1;  // AGC control.
    unsigned char AGCCTRL0;  // AGC control.
    unsigned char FSCAL3;    // Frequency synthesizer calibration.
    unsigned char FSCAL2;    // Frequency synthesizer calibration.
	unsigned char FSCAL1;    // Frequency synthesizer calibration.
    unsigned char FSCAL0;    // Frequency synthesizer calibration.
    unsigned char FSTEST;    // Frequency synthesizer calibration control
    unsigned char TEST2;     // Various test settings.
    unsigned char TEST1;     // Various test settings.
    unsigned char TEST0;     // Various test settings.
    unsigned char FIFOTHR;
    unsigned char IOCFG2;    // GDO2 output pin configuration
    unsigned char IOCFG0;    // GDO0 output pin configuration
    unsigned char PKTCTRL1;  // Packet automation control.
    unsigned char PKTCTRL0;  // Packet automation control.
    unsigned char ADDR;      // Device address.
    unsigned char PKTLEN;    // Packet length.
} RF_SETTINGS;

uchar CCxRead(uchar addr);
void CCxReadBurst(uchar addr, uchar* dataPtr, uint dataCount);
uchar CCxWrite(uchar addr, uchar dat);
void CCxWriteBurst(uchar addr, const uchar* dataPtr, uint dataCount);
uchar CCxStrobe(uchar addr);

//power on reset as discribed in  27.1 of cc1100 datasheet
void CCxPowerOnStartUp();

//configure registers of cc1100 making it work in specific mode
void CCxSetup(const RF_SETTINGS* settings);
uchar CCxReadSetup();

#endif
