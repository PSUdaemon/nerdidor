//  Firmware for rfBee 
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: June 4, 2010
//
//  This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program; 
//  if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA



#define FIRMWAREVERSION 11 // 1.1  , version number needs to fit in byte (0~255) to be able to store it into config
//#define FACTORY_SELFTEST
#define INTERRUPT_RECEIVE
//#define DEBUG 


#include "debug.h"
#include "globals.h"
#include "Config.h"
#include "CCx.h"
#include "rfBeeSerial.h"

#ifdef FACTORY_SELFTEST
#include "TestIO.h"  // factory selftest
#endif

#define GDO0 2 // used for polling the RF received data

void setup(){
    if (Config.initialized() != OK) 
    {
      Serial.begin(9600);
      Serial.println("Initializing config"); 
#ifdef FACTORY_SELFTEST
      if ( TestIO() != OK ) 
        return;
#endif 
      Config.reset();
    }
    Serial.begin(baudRateTable[Config.get(CONFIG_BDINDEX)]);
    //Serial.print(Config.get(CONFIG_BDINDEX),DEC);
    rfBeeInit();
    Serial.println("ok");
}

void loop(){
  // CCx_MCSM1 is configured to have TX and RX return to IDLE on completion or timeout
  // so we need to explicitly enable RX mode.
  if ((rfBeeMode == RECEIVE_MODE) || (rfBeeMode == TRANSCEIVE_MODE))
    if (serialMode != SERIALCMDMODE)
      CCx.Strobe(CCx_SRX);  
    
  if (Serial.available() > 0){
    if (serialMode == SERIALCMDMODE)
      readSerialCmd();
    else
      readSerialData();
  }
#ifdef USE_INTERRUPT_RECEIVE   
  if (state==RECV_WAITING)
     receiveData();
#else // polling mode
 if ( digitalRead(GDO0) == HIGH ) 
   receiveData();
#endif
 
}


void rfBeeInit(){
    DEBUGPRINT()
    
    CCx.PowerOnStartUp();
    loadSettings();
    serialMode=SERIALDATAMODE;
    rfBeeMode=RECEIVE_MODE;   
    
#ifdef USE_INTERRUPT_RECEIVE   
    state=IDLE;
    attachInterrupt(0, ISRVreceiveData, RISING);  //GD00 is located on pin 2, which results in INT 0
#else
    pinMode(GDO0,INPUT);// used for polling the RF received data
#endif 
}

// handle interrupt
#ifdef INTERRUPT_RECEIVE

void ISRVreceiveData(){
  DEBUGPRINT()
  
  if (state != IDLE)
    state=RECV_WAITING;
  else
    receiveData();
}

#endif


// read available txFifo size and handle underflow (which should not have occured anyway)
byte txFifoFree(){
  byte stat;
#ifdef USE_INTERRUPT_RECEIVE
  state=CHECKTX;
#endif
  
  CCx.Read(CCx_TXBYTES, &stat);
  // handle a potential TX underflow by flushing the TX FIFO as described in section 10.1 of the CC 1100 datasheet
  if (stat & 0x80){
    CCx.Strobe(CCx_SFTX);
    stat=CCx.Read(CCx_TXBYTES,&stat);
  }
  
#ifdef INTERRUPT_RECEIVE
  // did we miss a receive interrupt ?
  if (state==RECV_WAITING)
    receiveData();
  else
    state==IDLE;
#endif
  return (CCx_FIFO_SIZE - (stat & 0x7F));
}

// send data via RF
void transmitData(byte *serialData,byte len, byte srcAddress, byte destAddress){
  DEBUGPRINT()
  byte stat;
  
#ifdef USE_INTERRUPT_RECEIVE
  state=TRANSMIT;
#endif
  //Serial.println(len,DEC);
  CCx.Write(CCx_TXFIFO,len+2);
  CCx.Write(CCx_TXFIFO,destAddress);
  CCx.Write(CCx_TXFIFO,srcAddress);
  CCx.WriteBurst(CCx_TXFIFO,serialData, len); // write len bytes of the serialData buffer into the CCx txfifo
  CCx.Strobe(CCx_STX);
  delay(5);//give some time to STX,as the state would be changed to IDLE or RX in the loop.
#ifdef DEBUG
  serialData[len]='\0';
  Serial.println((char *)serialData);
#endif

#ifdef INTERRUPT_RECEIVE
  // did we miss a receive interrupt ?
  if (state==RECV_WAITING)
    receiveData();
  else
    state==IDLE;
#endif

}

// receive data via RF 
void receiveData(){
  DEBUGPRINT()
  
  byte size;
  byte rfData[CCx_PACKT_LEN];
  
  byte stat=CCx.Read(CCx_RXBYTES,&size);
    
  DEBUGPRINT(size)
  //packet format: payloadLen + dstAddr+ srcAddr +data + (RSSI + LQI)->optional
  //               1byte        1byte     1byte   nbyte  (1byte  1byte)
  //               payloadLen = length of addr and data = 2 + n
  //total len i =      1             +  (2 +   n)  +  (1    +   1)
  	
  if(size > 2 && size <= CCx_PACKT_LEN)
    CCx.ReadBurst(CCx_RXFIFO, rfData, size);
  else
    Serial.println("Error: Received invalid RF data size");
  // if the RSSI and LQI byte are present we need to subtract them from the length
  if (Config.get(CONFIG_RETURN_STATUS_BYTE) == 1)
    size -= 2;
  // playloadLen should be total size - 1
  if(rfData[0] == (size - 1)){
        // write dstAddr + srcAddr + data 
        Serial.write(&rfData[0], size); 
        // write the decoded RSSI value
        if( Config.get(CONFIG_RETURN_STATUS_BYTE) == 1 ) 
          Serial.print(byte(CCx.RSSIdecode(rfData[size])));
   }
   else{
     Serial.println("Error: Received invalid RF data");
   }
   // handle potential RX overflows by flushing the RF FIFO as described in section 10.1 of the CC 1100 datasheet
   if ((stat & 0xF0) == 0x60)//Modified by Icing. When overflows, STATE[2:0] = 110 
     CCx.Strobe(CCx_SFRX);
  
#ifdef INTERRUPT_RECEIVE
  // return to IDLE state
  state=IDLE;
#endif
}

void loadSettings(){
  // load the appropriate config table
  byte cfg=Config.get(CONFIG_CONFIG_ID);
//  Serial.println(cfg,DEC);
  CCx.Setup(cfg);  
  //CCx.ReadSetup();
  // and restore the config settings
  CCx.Write(CCx_ADDR, Config.get(CONFIG_MY_ADDR));
  //CCx.Write(CCx_PKTCTRL1, (Config.get(CONFIG_ADDR_CHECK) | 0x04 ));
  CCx.Write(CCx_PKTCTRL1, Config.get(CONFIG_ADDR_CHECK) | ((Config.get(CONFIG_RETURN_STATUS_BYTE))<<2));
  CCx.setPA(cfg,Config.get(CONFIG_PAINDEX));
}
