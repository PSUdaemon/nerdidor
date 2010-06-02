//  rfBeeSerial.pde serial interface to rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: May 22, 2010
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
  char data[2];
  int result=ERR;
  
#ifdef USE_INTERRUPT_RECEIVE   
        state=COMMAND;
#endif 
    
  // need at least 4 chars to continue
  if (Serial.available() < 4)
    delay(SERIALCMDDELAY); // wait a while for data to arrive
    
  if (Serial.available() >= 4){
    // read the AT
    data[0]=Serial.read();
    data[1]=Serial.read();
    if (strncmp("AT",data,2)==0){
      // read the command
      data[0]=Serial.read();
      data[1]=Serial.read();
      for(int i=0;i<=sizeof(atCommands);i++){
        // do we have a known command
        if (strncmp(atCommands[i].name,data,2)==0){
          // call the command function
          result=atCommands[i].function();
          break;
        }
      }
    }
  }
  
  if (result == OK)
    Serial.println("ok");
  if (result == ERR)
    Serial.println("error");
    
  Serial.flush();
  //serialMode=SERIALDATAMODE;// swith to data mode again
#ifdef USE_INTERRUPT_RECEIVE   
  // did we miss a receive interrupt ?
  if (state==RECV_WAITING)
    receiveData();
  else
    state==IDLE; 
#endif 

}

void readSerialData(){
  DEBUGPRINT()
  byte len;
  byte data;
  byte fifoSize=0;
  static int plus=0;
  static byte pos=0;
 
  // insert any plusses from last round
  for(int i=pos; i< plus;i++) //be careful, i should start from pos, -changed by Icing
    serialData[i]='+';
  
  len=Serial.available()+plus+pos;
  if (len > BUFFLEN ) len=BUFFLEN; //only process at most BUFFLEN chars
  
  // check how much space we have in the TX fifo
  fifoSize=txFifoFree();// the fifoSize should be the number of bytes in TX FIFO
  //Serial.println(fifoSize,DEC);
  if (len > fifoSize)  len=fifoSize;  // don't overflow the TX fifo
  
  for(int i=plus+pos; i< len;i++){
    data=Serial.read();
    serialData[i]=data;
    if (data == '+')
      plus++;
    else
      plus=0;
 
    if (plus == 3){
      len=i-2; // do not send the last 2 plusses
      plus=0;
      serialMode=SERIALCMDMODE;
      CCx.Strobe(CCx_SIDLE); 
      Serial.println("ok");
      //Serial.println(len,DEC);
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
    if(TRANSMIT_MODE == rfBeeMode || TRANSCEIVE_MODE== rfBeeMode){//only when TRANSMIT_MODE or TRANSCEIVE,transmit the buffer data,otherwise flush it.
        transmitData(&serialData[0],len,Config.get(CONFIG_MY_ADDR),Config.get(CONFIG_DEST_ADDR));//my addr and dest addr is changed here. -by Icing
    }
    else
      Serial.flush();
    pos=0; // serial databuffer is free again.
  }
}

int DA_command(){
  DEBUGPRINT()
  int destAddr;
  if (Serial.available()){
    if (getParamData(&destAddr,3) == OK)
      if (destAddr < 256){
        Config.set(CONFIG_DEST_ADDR,destAddr);
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_DEST_ADDR)); 
    return(OK); 
  }
  return ERR;
}

int MA_command(){
  DEBUGPRINT()
  int myAddr;
  if (Serial.available()){
    if (getParamData(&myAddr,3) == OK)
      if (myAddr < 256){
        CCx.Write(CCx_ADDR,myAddr);
        Config.set(CONFIG_MY_ADDR,myAddr);
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_MY_ADDR)); 
    return(OK); 
  }
  return ERR;
}

int AC_command(){
  DEBUGPRINT()
  int addrCheck;
  if (Serial.available()){
    if (getParamData(&addrCheck,1) == OK)
      if (addrCheck < 3){
        CCx.Write(CCx_PKTCTRL1, (addrCheck | ((Config.get(CONFIG_STATUS))<<2) ));
        Config.set(CONFIG_ADDR_CHECK,addrCheck);
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_ADDR_CHECK)); 
    return(OK); 
  }
  return ERR;
}

int PA_command(){
  DEBUGPRINT()
  int paIndex;
  byte cfg;
  if (Serial.available()){
    if (getParamData(&paIndex,1) == OK)
      if (paIndex < CCx_PA_TABLESIZE){
        cfg=Config.get(CONFIG_CONFIG_ID);
        CCx.setPA(cfg, (byte)paIndex);
        Config.set(CONFIG_PAINDEX ,paIndex);
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_PAINDEX)); 
    return(OK); 
  }
  return ERR;
}

int TH_command(){
  DEBUGPRINT()
  int threshold;
  if (Serial.available()){
    if (getParamData(&threshold,2) == OK)
      if (threshold < 33 ){
        Config.set(CONFIG_TX_THRESHOLD, threshold);
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_TX_THRESHOLD)); 
    return(OK); 
  }
  return ERR;
}

int DR_command(){
  int dataRate;
  byte cfg, newcfg;
  if (Serial.available()){
    if (getParamData(&dataRate,1) == OK)
      if (dataRate < sizeof(dataRateTable) ){
        cfg=Config.get(CONFIG_CONFIG_ID);
        if (dataRate == 0){
          switch(cfg){
            case 0:
            case 1:
              newcfg=0;
              break;
            case 2:
            case 3:
              newcfg=2;
              break;
            default:
              return ERR;
          }
        }
        else {
          switch(cfg){
            case 0:
            case 1:
              newcfg=1;
              break;
            case 2:
            case 3:
              newcfg=3;
              break;
            default:
              return ERR;
          }
        };
        if (newcfg != cfg){
          Config.set(CONFIG_DATARATE, dataRate);
          Config.set(CONFIG_CONFIG_ID,newcfg);
          loadSettings();
          // change config to newcfg
        }
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(dataRateTable[Config.get(CONFIG_DATARATE)]); //Modified by Icing
    return(OK); 
  }
  return ERR;
}

int BD_command(){
  DEBUGPRINT()
  int idx;
  if (Serial.available()){
    if (getParamData(&idx,1) == OK){
      if (idx < sizeof(baudRateTable)){
        Config.set(CONFIG_BDINDEX, idx);
        Serial.println("ok");
        Serial.flush();
        delay(1);      
        Serial.begin(baudRateTable[idx]);//modified by Icing
        return NOTHING;
      }
    }
  }
  else{
    // return current setting
    idx=Config.get(CONFIG_BDINDEX);
    Serial.println(baudRateTable[idx]);
    return OK;
  }
  return ERR;
}

int MD_command(){
  DEBUGPRINT()
  int md;

  if (Serial.available()){
    if (getParamData(&md,1) == OK)
      if (md < 5){ // CMD_MODE is removed by Icing
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
  else{
    // return current setting
    Serial.println(rfBeeMode);
    return(OK); 
  }
  return ERR;
}

int FV_command(){
  DEBUGPRINT()
  Serial.println(FIRMWAREVERSION);
  return OK;
}

int HV_command(){
  DEBUGPRINT()
  Serial.println((Config.get(CONFIG_HW_VERSION)),DEC);//modified by Icing
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
  if (Serial.available()){
    if (getParamData(&cf,1) == OK)
      if (cf < CCx.NrOfConfigs() ){
        Config.set(CONFIG_CONFIG_ID,cf); 
        // data rate is determined by the chosen configuration
        switch(cf){
          case 1:
          case 3:
              Config.set(CONFIG_DATARATE, 1);
              break;
          default:
              Config.set(CONFIG_DATARATE, 0);
              break;
        }
        loadSettings();
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_CONFIG_ID),DEC); //modified by Icing
    return(OK); 
  }
  return ERR;
}
int SI_command(){
  DEBUGPRINT()
  int si;
  if (Serial.available()){
    if (getParamData(&si,1) == OK)
      if (si < 2 ){
        Config.set(CONFIG_STATUS, si);
        CCx.Write(CCx_PKTCTRL1, Config.get(CONFIG_ADDR_CHECK) | (si<<2));
        return OK;
      }
  }
  else{
    // return current setting
    Serial.println(Config.get(CONFIG_STATUS)); 
    return(OK); 
  }
  return ERR;
}

int getParamData(int *result, int size){
  // try to read a number
  byte c;
  int value=0;
  boolean valid=false;
  
  do {
    c=Serial.read();
    if ( c== -1) // no data available
      break;
    else{
      if ((c < '0') || (c > '9'))
        break;
      else{
        valid=true;
        value=(value*10)+ (c -'0');
      }
    }
  } while (--size > 0);
  if (valid){
    *result=value;
    return OK;
  }
  return ERR;  
}
  
