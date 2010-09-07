#ifndef	_ATMEGA8_H
#define	_ATMEGA8_H

//#include "D:\Program files\arduino-0017\hardware\cores\arduino\HardwareSerial.h"

/*------------------------------------*/
#define uchar	unsigned char
#define uint		unsigned int
/*-----------------------------------*/

//extern char MchineState[][]={};

#define SPICS	PB2	// Port B bit 2 (pin14): chip select for CC
#define SPIDO	PB3	// Port B bit 3 (pin15): data out (data to CC1101)
#define SPIDI	PB4	// Port B bit 4 (pin16): data in (data from CC1101)
#define SPICLK	PB5	// Port B bit 5 (pin17): clock for CC1101

#define CC_GDO0	PD2	//INT0
#define CC_GDO2	PD3     //INT1

#define SET_SPICS_HIGH()	PORTB |= (1 << SPICS)
#define SET_SPICS_LOW()		PORTB &= ~(1 << SPICS)
#define READ_SPIDI()		PINB & (1 << SPIDI)

#define TEST_FLAG_EA  0
#define G_DESTADDR_EA  1
#define G_MYADDR_EA  2
#define G_LEVEL  3
#define G_DATARATE  4
#define G_BDINDEX  5
#define G_PAINDEX  6

//write or read eeprom, flag = 0-read, flag=1-write
void WRDEEPORM(uchar flag);

//recv and send data between mcu and ccx by SPI
void InitSPI();

//recv and send data between ccbee and host device by Uart
void InitUart(long baudRate);

//make ccx into working state
void InitCCx();

void MovUartDat(uchar *dstBuffer, int len);

//mov one frame of RF dat 2 uart buffer
//int  MovRFDat(uchar *dstBuffer);

//send data received from RF to host by uart
//void UartSend(uchar *inRFdata, int len);

//transmit data received from host by RF
void RFTransmit(uchar *inUartDat, int len, uchar myAddr, uchar dstAddr);

//interrupt service routine for uart receiving 
//void IntUartRecv();

//interrupt service routine for RF receiving
void ISRRFRecv();

//interrupt service routine for RF transmitting
//void IntRFTrans();	

//check if into CMD mode
int    CheckCMDMode(char *inUartDat, int len);
int TransProc();
int CmdProc( int len);

int RestoreDefaults();

int PrintMachineState();

void ChangeParam(uchar * param, char * inChar, int paramLen, int *errorFlag);

#endif

