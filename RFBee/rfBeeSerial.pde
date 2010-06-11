//  rfBeeSerial.pde serial interface to rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: June 7, 2010
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



#include "rfBeeSerial.h"

void readSerialCmd(){
  DEBUGPRINT()  
  int result;
  char data;
  static byte pos=0;
  
#ifdef USE_INTERRUPT_RECEIVE   
  state=COMMAND;
#endif  
 
  while(Serial.available()){
    result=NOTHING;
    data=Serial.read();
    serialData[pos++]=data; //serialData is our global serial buffer
    if (data == SERIALCMDTERMINATOR){
      if (pos > 3){ // we need 4 bytes
        result=processSerialCmd(pos);
      }
      else
        result=ERR;
      pos=0;
    }
    // check if we don't overrun the buffer, if so empty it
    if (pos > BUFFLEN){
      result=ERR;
      pos=0;
    }
    if (result == OK)
        Serial.println("ok");
    if (result == ERR)
        Serial.println("error");
  }
  
#ifdef USE_INTERRUPT_RECEIVE   
  // did we miss a receive interrupt ?
  if (state==RECV_WAITING)
    receiveData();
  else
    state=IDLE; 
#endif 
}

int processSerialCmd(byte size){
  DEBUGPRINT()  
  char cmd[2];
  AT_Command_Function_t function;
  
  // read the AT
  if (strncasecmp("AT",(char *)serialData,2)==0){
    // read the command
    for(int i=0;i<=sizeof(atCommands)/sizeof(AT_Command_t);i++){
      // do we have a known command
      if (strncasecmp_P((char *) serialData+2 , (PGM_P) pgm_read_word(&(atCommands[i].name)), 2)==0){
        // get the function pointer from PROGMEM
        function= (AT_Command_Function_t) pgm_read_word(&(atCommands[i].function));
        // call the command function
        return(function());  // return the result of the execution of the function linked to the command
      }
    }
  }
  return ERR;
}

void readSerialData(){
  DEBUGPRINT()
  byte len;
  byte data;
  byte fifoSize=0;
  static byte plus=0;
  static byte pos=0;
 
  // insert any plusses from last round
  for(int i=pos; i< plus;i++) //be careful, i should start from pos, -changed by Icing
    serialData[i]='+';
  
  len=Serial.available()+plus+pos;
  if (len > BUFFLEN ) len=BUFFLEN; //only process at most BUFFLEN chars
  
  // check how much space we have in the TX fifo
  fifoSize=txFifoFree();// the fifoSize should be the number of bytes in TX FIFO
  if (len > fifoSize)  len=fifoSize;  // don't overflow the TX fifo
  
  for(byte i=plus+pos; i< len;i++){
    data=Serial.read();
    serialData[i]=data;  //serialData is our global serial buffer
    if (data == '+')
      plus++;
    else
      plus=0;
 
    if (plus == 3){
      len=i-2; // do not send the last 2 plusses
      plus=0;
      serialMode=SERIALCMDMODE;
      CCx.Strobe(CCx_SIDLE); 
      Serial.println("ok, starting cmd mode");
      break;  // jump out of the loop, but still send the remaining chars in the buffer 
    }
  }
  
  if (plus > 0)  // save any trailing plusses for the next round
    len-=plus;
   
  // check if we have more input than the transmitThreshold, if we have just switched to commandmode send  the current buffer anyway.
  if ((serialMode!=SERIALCMDMODE)  && (len < Config.get(CONFIG_TX_THRESHOLD))){
    pos=len;  // keep the current bytes in the buffer and wait till next round.
    return;
  }
 
  if (len > 0){
    //only when TRANSMIT_MODE or TRANSCEIVE,transmit the buffer data,otherwise ignore
    if( rfBeeMode == TRANSMIT_MODE || rfBeeMode == TRANSCEIVE_MODE )                             
        transmitData(&serialData[0],len,Config.get(CONFIG_MY_ADDR),Config.get(CONFIG_DEST_ADDR)); 
    pos=0; // serial databuffer is free again.
  }
}

int DA_command(){
  DEBUGPRINT()
  int destAddr;
  
  byte result=getParamData(&destAddr,3);
  if (result == OK){
    if (destAddr < 256){
      Config.set(CONFIG_DEST_ADDR,destAddr);
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_DEST_ADDR),DEC); 
    return(OK); 
  }
  return ERR;
}

int MA_command(){
  DEBUGPRINT()
  int myAddr;
  
  byte result=getParamData(&myAddr,3);
  if (result == OK){
    if (myAddr < 256){
      CCx.Write(CCx_ADDR,myAddr);
      Config.set(CONFIG_MY_ADDR,myAddr);
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_MY_ADDR),DEC); 
    return(OK); 
  }
  return ERR;
}

int AC_command(){
  DEBUGPRINT()
  int addrCheck;
  
  byte result=getParamData(&addrCheck,1);
  if (result == OK){
    if (addrCheck < 3){
      CCx.Write(CCx_PKTCTRL1, (addrCheck | ((Config.get(CONFIG_RETURN_STATUS_BYTE))<<2) ));
      Config.set(CONFIG_ADDR_CHECK,addrCheck);
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_ADDR_CHECK),DEC); 
    return(OK); 
  }
  return ERR;
}

int PA_command(){
  DEBUGPRINT()
  int paIndex;
  byte cfg;
  
  byte result=getParamData(&paIndex,1);
  if (result == OK){
    if (paIndex < CCx_PA_TABLESIZE){
      cfg=Config.get(CONFIG_CONFIG_ID);
      CCx.setPA(cfg, (byte)paIndex);
      Config.set(CONFIG_PAINDEX ,paIndex);
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_PAINDEX),DEC); 
    return(OK); 
  }
  return ERR;
}

int TH_command(){
  DEBUGPRINT()
  int threshold;

  byte result=getParamData(&threshold,2);
  if (result == OK){
    if (threshold < 33 ){
      Config.set(CONFIG_TX_THRESHOLD, threshold);
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_TX_THRESHOLD),DEC); 
    return(OK); 
  }
  return ERR;
}

int BD_command(){
  DEBUGPRINT()
  int idx;
  
  byte result=getParamData(&idx,1);
  if (result == OK){
    if (idx < sizeof(baudRateTable)/sizeof(long)){
      Config.set(CONFIG_BDINDEX, idx);
      Serial.println("ok");
      Serial.flush();
      delay(1);      
      Serial.begin(pgm_read_dword(&baudRateTable[idx]));
      return NOTHING;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_BDINDEX),DEC);
    return OK;
  }
  return ERR;
}

int MD_command(){
  DEBUGPRINT()
  int md;

  byte result=getParamData(&md,1);
  
  if (result == OK){
    if (md < sizeof(RFBEEMODE)){ 
      rfBeeMode=(RFBEEMODE) md;
      // handle sleep mode, all other modes are handled in loop() and started from IDLE
      if (rfBeeMode==SLEEP_MODE){
        CCx.Strobe(CCx_SIDLE);
        CCx.Strobe(CCx_SPWD);
      }
      else
        CCx.Strobe(CCx_SIDLE);
      serialMode = SERIALDATAMODE;
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(rfBeeMode,DEC);
    return(OK); 
  }
  return ERR;
}

int FV_command(){
  DEBUGPRINT()
  Serial.println(((float) FIRMWAREVERSION)/10,1);
  return OK;
}

int HV_command(){
  DEBUGPRINT()
  Serial.println(((float)Config.get(CONFIG_HW_VERSION))/10,1);
  return OK;
}

int RS_command(){
  DEBUGPRINT()
  Config.reset();
  return OK;
}

int CF_command(){
  DEBUGPRINT()
  int cf;
  
  byte result=getParamData(&cf,1);
  if (result == OK){
    if (cf < CCx.NrOfConfigs() ){
      Config.set(CONFIG_CONFIG_ID,cf); 
      loadSettings();
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_CONFIG_ID),DEC); 
    return(OK); 
  }
  return ERR;
}

int SI_command(){
  DEBUGPRINT()
  int si;
  
  byte result=getParamData(&si,1);
  if (result == OK){
    if (si < 2 ){
      Config.set(CONFIG_RETURN_STATUS_BYTE, si);
      CCx.Write(CCx_PKTCTRL1, Config.get(CONFIG_ADDR_CHECK) | (si<<2));
      return OK;
    }
  }
  if (result == NOTHING){
    // return current setting
    Serial.println(Config.get(CONFIG_RETURN_STATUS_BYTE),DEC); 
    return(OK); 
  }
  return ERR;
}

int O0_command(){  // thats an o+zero
  DEBUGPRINT()
  serialMode = SERIALDATAMODE;
  return OK;
}

byte getParamData(int *result, int size){
  // try to read a number
  byte c;
  int value=0;
  boolean valid=false;
  int pos=4; // we start to read at pos 5 as 0-1 = AT and 2-3 = CMD
  
  if (serialData[pos] == SERIALCMDTERMINATOR )  // no data was available
    return NOTHING;
    
  while (size-- > 0){
    c=serialData[pos++];
    if ( c== SERIALCMDTERMINATOR)  // no more data available 
      break;
    if ((c < '0') || (c > '9'))     // illegal char
      return ERR;                     
    // got a digit
    valid=true;
    value=(value*10)+ (c -'0');
  } 
  if (valid){
    *result=value;
    return OK;
  }
  return ERR;  
}
  
