//  CommandControl.cpp  Class to control the RFBee based on the command 
//  received from the remote.
//  see http://www.seeedstudio.com/depot/ for details on the relayshield

//  Copyright (c) 2010  seeed technology inc.
//  Author: Icing Chang
//  Version: september 31, 2010
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

#include "WProgram.h"
#include "CommandControl.h"

CommandControl::CommandControl()
{
  for(int i = 0; i < CCx_PACKT_LEN; i++){
    rxBuffer[i] = 0;
  }
  
  rxLenInBuffer = 0;
  cmdHead = 0;
  cmdHeadLen = 0;
  cmdLen = 0;
  executeCmd = 0;
}
CommandControl::CommandControl(byte *initCmdHead,byte initCmdHeadLen, byte initCmdLen,void (*initExecuteCmd)(byte *cmdData))
{
  for(int i = 0; i < CCx_PACKT_LEN; i++){
    rxBuffer[i] = 0;
  }
  
  rxLenInBuffer = 0;
  
  cmdHead = (byte *)malloc(initCmdHeadLen);
  if(cmdHead){
    memcpy(cmdHead,initCmdHead,initCmdHeadLen); 
    cmdHeadLen = initCmdHeadLen;
    cmdLen = initCmdLen;
  }
  
  executeCmd = initExecuteCmd;
}

CommandControl::~CommandControl()
{

  if(cmdHead){
    free(cmdHead);
  }
    
}
void CommandControl::parseCmd(byte *rxData, byte rxLen){
  //copy new rx data to rx buffer, and increase the rx len in buffer
  if((rxLenInBuffer + rxLen) <= RX_BUFFER_LEN){
    memcpy(rxBuffer+rxLenInBuffer,rxData,rxLen);
    rxLenInBuffer += rxLen;
  }
  else{//copy new rx data to the beginning of rx buffer, as it is overflowing
    memcpy(rxBuffer,rxData,rxLen);
    rxLenInBuffer = rxLen;
  }
  
  //find the start flag of the command to be processed
  int startPos = 0;
  while(1){
    //Serial.println("here1");
    if((startPos = findCmd()) >= 0){
      //Serial.println(startPos);
      processCmd((byte)startPos);
    }
    else{
      break;
    }
  }
  
}

//find the command head, with the postion returned
int CommandControl::findCmd(){
  byte *p = rxBuffer;
  for(int i = 0; i < rxLenInBuffer; i++){
    if((strncmp((char *)(p+i),(char *)(cmdHead),cmdHeadLen) == 0)&&(i+cmdLen <= rxLenInBuffer)){
      return i;
    }
  }
  return -1;
}

//do special command processing,here we process the command to control the relay shield
void CommandControl::processCmd(byte headPos){
  //execute the command content
  executeCmd(rxBuffer+headPos+cmdHeadLen);
  //dump the processed command,and move the rest commands foward
  memcpy(rxBuffer,rxBuffer+headPos+cmdLen,rxLenInBuffer-headPos-cmdLen);
  rxLenInBuffer -= (headPos+cmdLen);
  
  
}
