//  rfBeeCore.cpp core routines for the rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: June 18, 2010
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "rfbeeCore.h"

// send data via RF
void transmitData(byte *txData,byte len, byte srcAddress, byte destAddress){
  DEBUGPRINT()
  byte stat;
  
  //Serial.println(len,DEC);
  CCx.Write(CCx_TXFIFO,len+2);
  CCx.Write(CCx_TXFIFO,destAddress);
  CCx.Write(CCx_TXFIFO,srcAddress);
  CCx.WriteBurst(CCx_TXFIFO,txData, len); // write len bytes of the serialData buffer into the CCx txfifo
  CCx.Strobe(CCx_STX);
  //delay(5);//give some time to STX,as the state would be changed to IDLE or RX in the loop.
#ifdef DEBUG
  txData[len]='\0';
  Serial.println((char *)txData);
#endif

}

// read available txFifo size and handle underflow (which should not have occured anyway)
byte txFifoFree(){
  byte stat;
  
  CCx.Read(CCx_TXBYTES, &stat);
  // handle a potential TX underflow by flushing the TX FIFO as described in section 10.1 of the CC 1100 datasheet
  if (stat & 0x80){
    CCx.Strobe(CCx_SFTX);
    stat=CCx.Read(CCx_TXBYTES,&stat);
  }
  
  return (CCx_FIFO_SIZE - (stat & 0x7F));
}

// receive data via RF, rxData must be at least CCx_PACKT_LEN bytes long
int receiveData(byte *rxData, byte *len, byte *srcAddress, byte *destAddress, byte *rssi , byte *lqi){
  DEBUGPRINT()
  
  byte size;
    
  byte stat=CCx.Read(CCx_RXBYTES,&size);
    
  //packet format: payloadLen + dstAddr+ srcAddr + data + RSSI + LQI
  //               1byte        1byte     1byte   nbyte  1byte  1byte
  //total len  =      1      +  (1   +       1) +   n  +  (1  +   1)
  //payloadLen = length of addresses and data = 2 + n 
  	
  if(size > 5 && size <= CCx_PACKT_LEN)
    CCx.Read(CCx_RXFIFO,len);
  else {
    errNo=1; // Error: Received invalid RF data size
    CCx.Strobe(CCx_SFRX); // flush the RX buffer
    return ERR;
  }
  
  // payloadLen should be total size - 3
  if (*len == size - 3){
    CCx.Read(CCx_RXFIFO,destAddress);
    CCx.Read(CCx_RXFIFO,srcAddress);
    *len -= 2;  // discard address bytes from payloadLen 
    CCx.ReadBurst(CCx_RXFIFO, rxData,*len);
    rxData[*len]='\0';
    CCx.Read(CCx_RXFIFO,rssi);
    *rssi=CCx.RSSIdecode(*rssi);
    CCx.Read(CCx_RXFIFO,lqi);
   }
   else{
     errNo=2; // Error: Received invalid RF data
     CCx.Strobe(CCx_SFRX); // flush the RX buffer
     return ERR;
   }
   // handle potential RX overflows by flushing the RF FIFO as described in section 10.1 of the CC 1100 datasheet
   if ((stat & 0xF0) == 0x60)//Modified by Icing. When overflows, STATE[2:0] = 110 
     CCx.Strobe(CCx_SFRX);
   return OK;
}

