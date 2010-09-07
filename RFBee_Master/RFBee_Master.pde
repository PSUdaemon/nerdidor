//  Firmware for controling the remote relay shield 
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: July 14, 2010
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


//command used for controlling the remote relay shield,
//and different commands will be sent in interval in the loop
//command format:"RCXY"
//RC-command head
//X-which relay to be contorlled,X could be '0'-relay1,'1'-relay2,'2'-relay3,'3'-relay4
//Y-which state the relay is to be set,Y could be '0'-set relay NO pin open,'1'-set realy NO pin close
//Note: This is just an demo on how to use RFBee in pair, and you can define your own commands.

byte relaysOnCmd[4][4] = {
 {'R','C','0','1'},
 {'R','C','1','1'},
 {'R','C','2','1'},
 {'R','C','3','1'},
};
byte relaysOffCmd[4][4] = {
 {'R','C','0','0'},
 {'R','C','1','0'},
 {'R','C','2','0'},
 {'R','C','3','0'},
};


//=======================================================================
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
    setUartBaudRate();
    rfBeeInit();
    Serial.println("ok");
}
//----------------------------------------------------------------------------------------
void loop(){

  //send cmmands to remote RFBee to control the relayshield in interval of 1s
  controlRemoteRelayShield();
  
  if (Serial.available() > 0){
    sleepCounter=1000; // reset the sleep counter
    if (serialMode == SERIALCMDMODE)
      readSerialCmd();
    else
      readSerialData();
  }
  
  if ( digitalRead(GDO0) == HIGH ) {
      writeSerialData();
      sleepCounter++; // delay sleep
  }
  sleepCounter--;
  
  // check if we can go to sleep again, going into low power too early will result in lost data in the CCx fifo.
  if ((sleepCounter == 0) && (Config.get(CONFIG_RFBEE_MODE) == LOWPOWER_MODE))
    DEBUGPRINT("low power on")
    lowPowerOn();
    DEBUGPRINT("woke up")
}
//===========================================================================================================

void rfBeeInit(){
    DEBUGPRINT()
    
    CCx.PowerOnStartUp();
    setCCxConfig();
   
    serialMode=SERIALDATAMODE;
    sleepCounter=0;
    
    attachInterrupt(0, ISRVreceiveData, RISING);  //GD00 is located on pin 2, which results in INT 0

    pinMode(GDO0,INPUT);// used for polling the RF received data

}

// handle interrupt
void ISRVreceiveData(){
  //DEBUGPRINT()
  sleepCounter=10;
}

//send cmmands to remote RFBee to control the relayshield in interval of 1s
void controlRemoteRelayShield()
{
  for(int i = 0; i < 4; i++){
    transmitData(relaysOnCmd[i],4,Config.get(CONFIG_MY_ADDR),Config.get(CONFIG_DEST_ADDR));  
  }
  delay(1000);
  for(int i = 0; i < 4; i++){
    transmitData(relaysOffCmd[i],4,Config.get(CONFIG_MY_ADDR),Config.get(CONFIG_DEST_ADDR));  
  }
  delay(1000);
}

