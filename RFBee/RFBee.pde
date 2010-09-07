/*************************************

project:		RFBee
file name:	        RFBee.pde
function:		wireless recv&trans;
			frequency in 868/915MHz;
			UART interface;
			Transparent operation;
author:		Icing
time:		2009-11-11 18:02:27
note: 		ccx reperesent cc1101
		atmegax repereseneats atmega168
		       
Copyright (c) 2009 Seeedstudio.  All right reserved.

http://seeedstudio.com/

**************************************/

/*********remember I have changed the HardwareSerial.c and .h file*******/


#include <avr/io.h>
#include <EEPROM.h>
#include "Ccx.h"
#include "Atmegax.h"
#include "TestIO.h"

//#include "D:\Program files\arduino-0017\hardware\cores\arduino\HardwareSerial.h"

extern ring_buffer rx_buffer;//defined in HardwareSerial.c

//#define DEBUG
//#define DEBUG_READ
//#define DEBUG_SETUP

#define CCX_PACKT_LEN 64

//RF default settings
#define FQ_915_DEFAULT_SETTING
//#define FQ_868_DEFAULT_SETTING


//Modes of operation 
#define IDLE_MODE       '0'
#define TRANS_MODE    '1'         
#define RECV_MODE      '2'
#define TRANSV_MODE '3'
#define SLEEP_MODE    '4'
#define CMD_MODE       '5'



//Maximum number of characters that will fit in an RF packet
#define MAX_PACKET_SIZE 32


#define TX_BUFFER_SIZE 64 
#define RF_TRANS_BUFFER_SIZE 64
#define RF_RECV_BUFFER_SIZE 64

/*global variable*/
//uchar g_transmitRdy = 0;//there are data ready for transmitting,set 1 as if receiving data from uart
//uchar g_recvComplt = 0;//there are data ready for send to host through uart,set 1 as if RF receiving completes,


long g_baudRate[] = {9600,19200,38400,115200};
uchar g_bdIndex = 0;//baudrate index
uchar g_paIndex = 7;//power amplifier index

//uart rx and RF transmitt buffer
/*
----> uart rx ring buffer----->RF trans buffer----->
<----uart tx buffer<-----RF receiv ring buffer<----
*/

ring_buffer & g_rxBuffer = rx_buffer;
uchar g_RFTransBuffer[RF_TRANS_BUFFER_SIZE];

//RF receive and uart tx buffer
uchar g_txBuffer[TX_BUFFER_SIZE];

//address of each end,0~255
uchar g_destAddr = 0;
uchar g_myAddr = 0;
uchar g_checkAddr = 0;

char g_mode = IDLE_MODE;//Intial modes of operation

int g_plusNum = 0;// number of '+'

uchar g_level = 4;//transmitt level,transmitt data when g_level >= 4  

//uchar SSIPin = 9;//receiving signal strength

/*uchar PC1_Pin = 15;//for testing CCA,
uchar PC0_Pin = 14;//for testing 

uchar GDO2 = 3;*/

uchar GDO0 = 2;// used for polling the RF received data

uchar g_dataRate = 0;

uchar testFlag = 0;//save to and load from address 0 of eeprom 

void setup()
{       
	//recv and send data between mcu and ccx by SPI
	InitSPI();

        WRDEEPORM(0);//read from eeprom
        if(0xAA != testFlag)
        {
            InitUart(g_baudRate[0]);//baudrate 9600
            
            if(0 == TestIO())//test ok
            {
              Serial.println("IO ok");
              WRDEEPORM(1);//write to eeprom
            }
            else
            {
                Serial.println("IO error");
            }
            return;
        }

        //recv and send data between ccbee and host device by Uart
	InitUart(g_baudRate[g_bdIndex]);

	//make ccx into idle state
	InitCCx();
        

       Serial.println("ok"); 
       
       pinMode(GDO0,INPUT);// used for polling the RF received data
}


void loop()
{
    int static no_CCA_count = 0;
    
    int cmdOut = 0;
    
    //add sate check,avoiding RFBee into dead state
    int x = CCxRead(CCx_MARCSTATE);
    if(x == 22)//TXFIFO_UNDERFLOW
      CCxStrobe(CCx_SFTX);
    else if(x == 17)//RXFIFO_OVERFLOW
      CCxStrobe(CCx_SFRX);
    else if(x == 1&& ( RECV_MODE ==g_mode || TRANSV_MODE == g_mode) )//IDLE
      CCxStrobe(CCx_SRX);
            
    //change interupt way to polling mode,making RFBee more stable,however may lose some packet in high speed.
    if(HIGH == digitalRead(GDO0)) 
      ISRRFRecv();
    
    uchar RxDatLen = Serial.available();

    //read data continually from the UART,if avilable parse the content
    if(  RxDatLen > 0)
    {
        if(CheckCMDMode((char *)(g_rxBuffer.buffer+g_rxBuffer.tail),RxDatLen))
        {
          return;
        }
        
        if(g_plusNum)
        {
          return;
        }
    
        if(TRANS_MODE == g_mode || TRANSV_MODE == g_mode)
        {
          TransProc();
        }
        else if(CMD_MODE == g_mode)
        {
           cmdOut = CmdProc(RxDatLen);
           if(0 == cmdOut)
           {
               Serial.flush();
               Serial.print("ok\r\n");
            }
            else if(-1 == cmdOut)
            {
                Serial.flush();
                Serial.print("error\r\n");
            }
        }
    }

}




