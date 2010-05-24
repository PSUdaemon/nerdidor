/*************************************
project:		RFBee
file name:	Atmegax.pde
function:		Implementation of SPI,UART,
			TIMER,INTERRUPT,etc..
author:		Icing
Copyright (c) 2009 Seedstudio.  All right reserved.
time:		2009-11-11 18:02:27
**************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Atmegax.h"
#include "CCx.h"

extern RF_SETTINGS rfSettings;//1.2k
extern RF_SETTINGS rfSettings_1;//76.8k
extern uchar paTable[];
extern long g_baudRate[];
extern uchar g_bdIndex;
extern uchar g_paIndex;

extern ring_buffer & g_rxBuffer;
extern uchar g_RFTransBuffer[RF_TRANS_BUFFER_SIZE];

//RF receive and uart tx buffer
//extern ring_buffer g_RFRcvBuffer;
extern uchar g_txBuffer[TX_BUFFER_SIZE];

//address of each end,0~255
extern uchar g_destAddr;
extern uchar g_myAddr;
extern uchar g_checkAddr;

extern char g_mode;//modes of operation
extern int g_plusNum;//number of '+'
extern uchar g_level;//transmitt level

extern uchar testFlag;

//extern uchar SSIPin;
//int testPin = 1;

//write or read eeprom, flag = 0-read, flag=1-write
void WRDEEPORM(uchar flag)
{
  if(1 == flag)
  {
    EEPROM.write(0,0xAA);
    EEPROM.write(1,0x00);
    EEPROM.write(2,0x00);
    EEPROM.write(3,0x00);
    EEPROM.write(4,0x04);
    EEPROM.write(5,0x00);
    EEPROM.write(6,0x00);
    EEPROM.write(7,0x07);
  }
  else if(0 == flag)
  {
    testFlag = EEPROM.read(0);
    g_destAddr = EEPROM.read(1);
    g_myAddr = EEPROM.read(2);
    g_checkAddr = EEPROM.read(3);
    g_level = EEPROM.read(4);
    g_dataRate = EEPROM.read(5);
    g_bdIndex = EEPROM.read(6);
    g_paIndex = EEPROM.read(7);
  }
}

//recv and send data between mcu and ccx by SPI
void InitSPI()
{
	DDRB &= ~(1<< SPIDI);
	DDRB |= (1 << SPIDO);
	DDRB |= (1 << SPICLK);
	DDRB |= (1 << SPICS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1)| (1 << SPR0) ;//SPICLK=CPU/64
	SPSR = 0x00;	

	//set for cc1100 power on
	PORTB |= (1 << SPICS);	// set chip select to high (CC is NOT selected)
	PORTB &= ~(1 << SPIDO);	// data out =0
	PORTB |= (1 << SPICLK); // clock out =1

}


//recv and send data between ccbee and host device by Uart
void InitUart(long baudRate)
{
	//set baud rate
	Serial.begin(baudRate);
        #ifdef DEBUG
        Serial.print("inital uart ok\r\n");
        #endif
}

//recv and send data between ccbee and host device by I2C as well
void InitI2C()
{

}

int PrintMachineState()
{
        int x;
         x = CCxRead(CCx_MARCSTATE);
        Serial.print("machine state:");
        Serial.print(x);
        Serial.print("\r\n\r\n");
        return x;
}

//Initialze ccx into idle state
void InitCCx()
{
        int x = 0;
        
	CCxPowerOnStartUp();//ccx into IDLE state
        
        rfSettings.PKTCTRL1 = 0x04|g_checkAddr;
        rfSettings.ADDR = g_myAddr;
	CCxSetup(&rfSettings);//configure registers

	#ifdef DEBUG_SETUP
	CCxReadSetup();
	#endif
	
	CCxWriteBurst(CCx_PATABLE, &paTable[g_paIndex], 1); //confgure power amplifer 

	/*CCxWrite(CCx_MCSM2 ,0x01);
       CCxWrite(CCx_WOREVT1 ,0x28);
    	CCxWrite(CCx_WOREVT0 ,0xA0);
    	CCxWrite(CCx_WORCTRL ,0x38);
        CCxStrobe(CCx_SRX);//ccx into wake on radio state
        */
        #ifdef DEBUG_
        PrintMachineState();
        #endif
        
	CCxStrobe(CCx_SIDLE);//ccx into idle state
        /*if(TRANSV_MODE == g_mode)
        {
          CCxWrite(CCx_MCSM1 ,   0x3F );//RXOFF_MODE and TXOFF_MODE stay in RX
          CCxStrobe(CCx_SFTX);
           CCxStrobe(CCx_SFRX);
           CCxStrobe(CCx_SRX);
        }*/
        //CCxStrobe(CCx_STX);
        #ifdef DEBUG
        delay(1);
        PrintMachineState();
        #endif
}


//transmit data received from host by RF
void RFTransmit(uchar *inUartDat, int len, uchar myAddr, uchar dstAddr)
{
	int transLen = 0;
	g_RFTransBuffer[0] = len + 2;
	g_RFTransBuffer[1] = dstAddr;
       g_RFTransBuffer[2] = myAddr;
       //memcpy(&g_RFTransBuffer[3],inUartDat,len);
       
	transLen = len + 3;
       
       #ifdef DEBUG
	//CCxStrobe(CCx_SIDLE);
       #endif

       //wait untill CCx_TX buffer is empty
       //while(CCxRead(CCx_TXBYTES));
       //CCxStrobe(CCx_SFTX);//add by icing, 2010/3/16

        //write all data to CCx_TX buffer
	CCxWriteBurst(CCx_TXFIFO,g_RFTransBuffer, transLen); 
        
        
	#ifdef DEBUG_
        int x = CCxRead(CCx_TXBYTES);
        Serial.print("\r\n\r\nbefore trans txbuffer data:");
        Serial.print(x);
        //Serial.print("\r\nSTX command start\r\n");
        //Serial.write(&g_RFTransBuffer.buffer[2],transLen-2);
        //Serial.print("\r\n");
        x = CCxRead(CCx_MARCSTATE);
        Serial.print("before trans state: ");
        Serial.print(x);
        Serial.print("\r\n");
	#endif
	
        CCxStrobe(CCx_STX);
        //delayMicroseconds(100);
        //CCxStrobe(CCx_STX);
        //delay(5);
        #ifdef DEBUG
        //delay(10);
        //int x = CCxRead(CCx_TXBYTES);
        //Serial.print("after trans txbuffer data:");
        //Serial.print(x);
        //PrintMachineState();
        #endif
}


//interrupt service routine for RF receiving
void ISRRFRecv()
{
  	int i;
        uint8_t rssi_dec;
        int16_t rssi_dBm;
        uint8_t rssi_offset = 74;
  
       uchar tempBuffer[CCX_PACKT_LEN];
       
	#ifdef DEBUG
       Serial.print("\r\n");
	#endif

	
       i = CCxRead(CCx_RXBYTES);

	#ifdef DEBUG_
	Serial.print("RXed bytes: ");
	Serial.print(i);
	Serial.print("\r\n");
	#endif

	//packet format: payloadLen + dstAddr+ srcAddr +data + RSSI + LQI
	//               1byte        1byte     1byte   nbyte  1byte  1byte
	//               payloadLen = length of addr and data = 2 + n
	//total len i =      1             +  (2 +   n)  +  1    +   1
	
   	if(i > 2 && i < CCX_PACKT_LEN)
   	{
       	 CCxReadBurst(CCx_RXFIFO, tempBuffer, i);
   	} 
        //Serial.print(int(tempBuffer[0]));
       if(tempBuffer[0] == (i -3))//playloadLen should = total len -3
       {
	    //StoreRFDat(tempBuffer,i,&g_RFRcvBuffer);
              /*Serial.print((int)tempBuffer[0]);
              Serial.print(" ");
              Serial.print((uint)tempBuffer[1]);
              Serial.print(" ");
              Serial.print((uint)tempBuffer[2]);
              Serial.print(" ");
              Serial.write(&tempBuffer[3], i-5);
              Serial.print(" ");
              Serial.print((uint)tempBuffer[i-2]);
              Serial.print(" ");
              Serial.print((uint)tempBuffer[i-1]);
              */
              rssi_dec=tempBuffer[i-2];
              if (rssi_dec >= 128)
              {
                rssi_dBm = (int16_t)((int16_t)( rssi_dec - 256) >> 1) - rssi_offset;
	      }
              else
              {
                rssi_dBm = (rssi_dec >> 1) - rssi_offset;
               }
               
              ///////////////////////
              tempBuffer[0] += 1; 
              Serial.write(&tempBuffer[0], i-2);
              Serial.write((uint8_t *)&rssi_dBm,1);
              
              ///////////////////////
              /*Serial.write(&tempBuffer[3], i-5);
              Serial.print(rssi_dBm);
              Serial.print("dBm");
              */
              
              ///////////////////////
              //testPin = 1 - testPin;
              //digitalWrite(SSIPin, testPin);
        }
      
        
          //CCxStrobe(CCx_SRX);


}





void MovUartDat(uchar *dstBuffer, int len)
{
    for(int i = 0; i< len; i++)
    {
        dstBuffer[i] = Serial.read();
    }
}


//packaging the data in the uart rx buffer,and mov them to RF transmit buffer,then transmit each packet
int TransProc()
{
    int rxLen = Serial.available();
        
    if(0 == rxLen)
    {
      return 0;
    }
    
    //add checking CMD mode here
    CheckCMDMode((char *)(g_rxBuffer.buffer+g_rxBuffer.tail),rxLen);    
    if(g_plusNum)
    {
         return 0;
    }
    
    //transmitt data,only when rexLen bigger than g_level
    if(rxLen < g_level)
    {
      return 0;
    }
    delay(10);
    //rxLen = Serial.available();
    
    #ifdef DEBUG
    Serial.print("rxLen:");
    Serial.print(rxLen);
    Serial.print("\r\n");
    #endif

    //if(rxLen <= MAX_PACKET_SIZE)
    {
        MovUartDat(&g_RFTransBuffer[3],g_level);
        RFTransmit(g_RFTransBuffer, g_level,g_myAddr,g_destAddr);
        return 1;
    }

    //if more than one packet data in the rx_buffer,send them in several times
    /*int packNum = rxLen>>5;//rxLen/MAX_PACKET_SIZE;
    for(int i = 0; i < packNum; i++)
    {
        MovUartDat(g_RFTransBuffer,MAX_PACKET_SIZE);
        RFTransmit(g_RFTransBuffer, MAX_PACKET_SIZE,g_myAddr,g_destAddr);
    }

    //send remains
    int reLen = rxLen -(packNum<<5);
    MovUartDat(g_RFTransBuffer,reLen);
    RFTransmit(g_RFTransBuffer, reLen,g_myAddr,g_destAddr);
    
    return 1;*/
}

//check if enter command mode
int CheckCMDMode(char *inUartDat, int len)
{
    //Serial.print(len);
    //Serial.write(&g_rxBuffer.buffer[g_rxBuffer.tail],len);
    //Serial.print("\r\n");
    
    
    
    for(int i = g_plusNum; i < len; i++)
    {
      if( '+'== inUartDat[i] )
      {
        g_plusNum++;
        if(3  == g_plusNum)
        {
           g_mode = CMD_MODE;
           g_plusNum = 0;
           Serial.flush();
           CCxStrobe(CCx_SIDLE);
           //Serial.print(g_rxBuffer.head);
           //Serial.print("enter AT command mode\r\n");
            Serial.println("ok");
           return 1;  
        }
      }
      else
      {
          g_plusNum = 0;
      }
    }

    /*if(g_mode != CMD_MODE && g_mode != TRANS_MODE && len > 5)
    {
      Serial.flush();
    }*/
    return 0;
}

//process AT command,format: AT + Comand(ASCII) + parameter(optional,Hex)
//Example: ATDT12,change the RF module Destination Address to 12
/*
supported command:

addressing:
DT -destination address              (0~255)
MY - my address                       (0~255)
AC - address check option            (0-no, 1-address check,2-address check and 0 broadcast )

power:
PA - power amplifer                   (0: -30,1: -20,2: -15,3: -10, 4: 0,5: 5,6: 7,7: 10 )

uart:
BD - uart baudrate                    (0-9600,1-19200,2-38400,3-115200)

mode:
MD - working mode                  (0-idle,1-trans,2-receiv,3-sleep,4-command)

TH- threshold of transmitting         (0~32)

RF datarate
DR - RF datarate                  (0-1.2k,1-76.8k)


Diagnostics:
FV - firmware version
HV - hardware version
RS - restore default settings
*/

/*
return value" 0-ok,-1-error,1-nothing
*/

int CmdProc( int len)
{
    int errorFlag = 1;
    int paramLen;
    //char tempBuffer[10];
    //Serial.print(len);
    //Serial.write(&g_rxBuffer.buffer[g_rxBuffer.tail],len);
    
    char *p = (char *)&g_rxBuffer.buffer[g_rxBuffer.tail];  

	for(int i = 0; i < len; i++)
	{
		if('A' == p[i])
		{
			delay(100);
			
			len = Serial.available();
			if(len < i + 4)
			{
				//Serial.flush();
				return -1;
			}
			else
			{  
                                paramLen = len - i - 4;
				if(strncmp(&p[i],"AT",2) == 0)
				{
                                        if(strncmp(&p[i+2],"RS",2) == 0)
                                        {
                                          RestoreDefaults();
                                          errorFlag = 0;
                                        }
                                        else if(strncmp(&p[i+2],"DR",2) == 0)//change RF datarate
                                        {
                                          if(0 == paramLen)
                                          {
                                            if(0 == g_dataRate)
                                            {
                                              Serial.println("76800");
                                              errorFlag = 0;
                                            }
                                            else
                                            {
                                              Serial.println("1200");
                                              errorFlag = 0;
                                            }
                                          }
                                          else 
                                          {
                                            if('0' == p[i+4]) //1.2
                                            {
                                              g_dataRate = 0;   
                                              CCxSetup(&rfSettings);
                                              errorFlag = 0;
                                            }
                                            else
                                            {
                                              g_dataRate = 1;
                                              CCxSetup(&rfSettings_1);
                                              errorFlag = 0;
                                            }
                                            EEPROM.write(5,g_dataRate);
                                          } 
                                        }
                                        else if(strncmp(&p[i+2],"FV",2) == 0 || strncmp(&p[i+2],"HV",2) == 0)//show hardware and firmware version
                                        {
                                          Serial.println("v1.0");
                                          errorFlag = 0;
                                        }
                                        else if(strncmp(&p[i+2],"TH",2) == 0)//change threshold of transmitting 
        				{
                                            ChangeParam(&g_level, &p[i+4], paramLen, &errorFlag);
                                            EEPROM.write(4,g_level);
				        }
                                        else if(strncmp(&p[i+2],"AC",2) == 0)//adress check
        				{
                                            if( 0 == paramLen)
                                            { 
                                                 //g_checkAddr = rfSettings.PKTCTRL1&0x01;
                                                 Serial.println((int)g_checkAddr);
                                                 errorFlag = 0;
                                            }
				            else if(p[i+4] >= '0' && p[i+4] <= '2')
				            {
                                                   g_checkAddr = p[i+4]-'0';
                                                   rfSettings.PKTCTRL1 = 0x04|g_checkAddr;
				                   CCxWrite(CCx_PKTCTRL1, rfSettings.PKTCTRL1);
				                   errorFlag = 0;
                                                   EEPROM.write(3,g_checkAddr); 
					      }
					      else
				            {
				                 errorFlag = -1;
				            }
				        }
                                        else if(strncmp(&p[i+2],"DA",2) == 0)//change address of destination 
        				{
                                            ChangeParam(&g_destAddr, &p[i+4], paramLen, &errorFlag);
                                            EEPROM.write(1,g_destAddr);
				        }
                                        else if(strncmp(&p[i+2],"MA",2) == 0)//change address of destination 
        				{
                                            ChangeParam(&g_myAddr, &p[i+4], paramLen, &errorFlag);
                                            CCxWrite(CCx_ADDR,g_myAddr);
                                            EEPROM.write(2,g_myAddr);
				        }
                                        else if(strncmp(&p[i+2],"PA",2) == 0)
        				{
                                            if( 0 == paramLen)
                                            { 
                                                 Serial.println((int)g_paIndex);
                                                 errorFlag = 0;
                                            }
				            else if(p[i+4] >= '0' && p[i+4] <= '7')
				            {
                                                   g_paIndex = p[i+4]-'0';
				                   CCxWriteBurst(CCx_PATABLE, &paTable[g_paIndex], 1); //confgure power amplifer 
				                   errorFlag = 0;
                                                   EEPROM.write(7,g_paIndex);
					      }
					      else
				            {
				                 errorFlag = -1;
				            }
				        }
					else if(strncmp(&p[i+2],"BD",2) == 0)
        				{
				             if( 0 == paramLen)
                                            { 
                                                 Serial.println(g_baudRate[g_bdIndex]);
                                                 errorFlag = 0;
                                            }
				            else if(p[i+4] >= '0' && p[i+4] <= '3')
				            {
				                   //g_baudRate = p[i+4];
                                                    Serial.println("ok");
                                                    Serial.flush();
                                                    delay(1);
                                                    g_bdIndex = p[i+4]-'0';
                                                    Serial.begin(g_baudRate[g_bdIndex]);
                                                    errorFlag = 1;
                                                    EEPROM.write(6,g_bdIndex);
					      }
					      else
				            {
				                 errorFlag = -1;
				            }
				        }
				        else if(strncmp(&p[i+2],"MD",2) == 0)
				        {
				              if( 0 == paramLen)
                                            { 
                                                 Serial.println(g_mode);
                                                 errorFlag = 0;
                                            }
				            else if(p[i+4] >= '0' && p[i+4] <= '5')
				              {
				                   g_mode = p[i+4];
				                    //Serial.print("MD ok\r\n");
				                    errorFlag = 0;
				              }
				              else
				              {
				                    //Serial.print("MD error\r\n");
					             errorFlag = -1;
				              }
              
				            //strobe into different mode
				            if(0 == errorFlag)
				            {   //Serial.print(g_mode);
                                                //Serial.write(&g_rxBuffer.buffer[g_rxBuffer.tail],len);
				            switch (g_mode)
				            {
				                case IDLE_MODE:
				                    CCxStrobe(CCx_SIDLE);
				                    //Serial.print("enter idle mode\r\n");
				                    //PrintMachineState();
				                    break;
				                case TRANS_MODE:
				                    //FlushBuffer(&g_rxBuffer);
                                                    g_rxBuffer.head = g_rxBuffer.tail;
				                    CCxStrobe(CCx_SIDLE);
				                    delay(1);
                                                    CCxWrite(CCx_MCSM1 ,   0x00 );//TXOFF_MODE->stay in IDLE
				                    CCxStrobe(CCx_SFTX);
				                    //Serial.print("enter RFtrans mode\r\n");
				                    //Serial.print(g_rxBuffer.tail);
				                    //Serial.print(g_rxBuffer.head);
				                    break;
				                case RECV_MODE:
				                    CCxStrobe(CCx_SIDLE);
                                                    delay(1);
                                                    CCxWrite(CCx_MCSM1 ,   0x0C );//RXOFF_MODE->stay in RX
				                    CCxStrobe(CCx_SFRX);
				                    CCxStrobe(CCx_SRX);
				                    //Serial.print("enter RFrecv mode\r\n");
				                    break;
                                                case TRANSV_MODE:
						    CCxStrobe(CCx_SIDLE);
                                                    delay(1);
                                                    CCxWrite(CCx_MCSM1 ,   0x0F );//RXOFF_MODE and TXOFF_MODE stay in RX
						    CCxStrobe(CCx_SFTX);
				                    CCxStrobe(CCx_SFRX);
				                    CCxStrobe(CCx_SRX);
                                                    break;
				                case SLEEP_MODE:
				                    CCxStrobe(CCx_SIDLE);
				                    CCxStrobe(CCx_SPWD);
				                    //PrintMachineState();
				                    //Serial.print("enter sleep mode\r\n");
				                    break;
				                case CMD_MODE:
				                    CCxStrobe(CCx_SIDLE);
				                    //Serial.print("enter AT command mode\r\n");
				                    break;
				                default:
				                    //Serial.print("unsupported AT command\r\n");
			
	                                              break;
					        }
				            	}
      					}
					else
					{
				       	 errorFlag = -1;
					}
				}
			return errorFlag;
			}
		}
	}

	return errorFlag;
	
    
}


int RestoreDefaults()
{
    testFlag = 0xAA;
    g_destAddr = 0;
    g_myAddr = 0;
    g_checkAddr = 0;
    g_level = 4;
    g_dataRate = 0;
    g_bdIndex = 0;
    g_paIndex = 7;
    WRDEEPORM(1);//write to eeprom
    
    return 0;
}

void ChangeParam(uchar * param, char * inChar, int paramLen, int *errorFlag)
{
  if( 0 == paramLen)
  { 
   Serial.write((uint8_t *)(param),1);
   *errorFlag = 0;
   }
   else if (1 == paramLen)
  {
    *param = inChar[0] - '0';
    *errorFlag = 0;
   }
   else if (2 == paramLen)
   {
      *param = (inChar[0] - '0')*10 + (inChar[1] - '0');
      *errorFlag = 0;
    }
   else if (3 == paramLen)
   {
     *param = (inChar[0] - '0')*100 + (inChar[1] - '0')*10 + (inChar[2] - '0');
     *errorFlag = 0;
    }
   else
    {
     *errorFlag = -1;
    }
}












