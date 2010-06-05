//  rfBeeSerial.h serial interface to rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: June 4, 2010
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


#ifndef RFBEESERIAL_H
#define RFBEESERIAL_H 1

#include "debug.h"
#include "globals.h"
#include "Config.h"
#include "CCx.h"

#define BUFFLEN 64
#define SERIALCMDMODE 1
#define SERIALDATAMODE 0
#define SERIALCMDTERMINATOR 13  // use <CR> to terminate commands

void readSerialCmd();
void readSerialData();

byte serialData[BUFFLEN+1]; // 1 extra so we can easily add a /0 when doing a debug print ;-)
byte serialMode;

// RFbee AT commands

// Macro to ease command definition
#define COMMAND(NAME)  { #NAME, NAME ## _command }


int DA_command();
int MA_command();
int AC_command();
int PA_command();
int TH_command();
int BD_command();
int MD_command();
int FV_command();
int HV_command();
int RS_command();
int CF_command();
int SI_command();  // added by Icing
int O0_command();  // thats o+zero

// Supported commands, Commands and parameters in ASCII
// Example: ATDT0E means: change the RF module Destination address to 14
typedef struct
{
  char *name;
  int (*function) (void);
}  AT_COMMAND;

AT_COMMAND atCommands[] =
{
// Addressing:
  COMMAND (DA), // Destination address              (0~255)
  COMMAND (MA), // My address                       (0~255)
  COMMAND (AC), // address check option             (0: no, 1: address check , 2: address check and 0 broadcast )
// RF
  COMMAND (PA), // Power amplifier                  (0: -30 , 1: -20 , 2: -15 , 3: -10 , 4: 0 , 5: 5 , 6: 7 , 7: 10 )
  COMMAND (TH), // TH- threshold of transmitting    (0~32) 
// Serial
  COMMAND (BD), // Uart baudrate                    (0: 9600 , 1:19200, 2:38400 ,3:115200)
// Mode 
  COMMAND (MD), // CCX Working mode                 (0:idle , 1:transmit , 2:receive, 3:transceive,4:sleep)
  COMMAND (O0), // go back to online mode
// Diagnostics:
  COMMAND (FV), // firmware version
  COMMAND (HV), // hardware version
  COMMAND (RS), // restore default settings
  COMMAND (CF), // select CCx configuration        (0: 915 Mhz - 76.8k, 1: 915 Mhz - 1.2k, 2: 868 Mhz - 76.8k, 3: 868 Mhz - 1.2k, 4: 433 Mhz)
// status  
  COMMAND (SI), //enable status info bytes RSSI and LQI (1:enable,0:disable). -Added by Icing
};

long baudRateTable[] = {9600,19200,38400,115200};
long dataRateTable[] = {76800,1200};//Modified by Icing

enum RFBEEMODE {
   IDLE_MODE=0,
   TRANSMIT_MODE=1,       
   RECEIVE_MODE=2, 
   TRANSCEIVE_MODE=3,
   SLEEP_MODE=4,   
} rfBeeMode;

#ifdef INTERRUPT_RECEIVE
volatile enum STATE {
  IDLE,
  CHECKTX,
  TRANSMIT,
  RECV_WAITING,
  COMMAND,
} state;
#endif

#endif
