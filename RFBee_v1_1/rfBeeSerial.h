//  rfBeeSerial.h serial interface to rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: June 6, 2010
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
#include <avr/pgmspace.h>

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
#define AT_COMMAND(NAME)  { NAME ## _label , NAME ## _command }


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

// Need to define the labels outside the struct :-(
static char DA_label[] PROGMEM="DA";
static char MA_label[] PROGMEM="MA";
static char AC_label[] PROGMEM="AC";
static char PA_label[] PROGMEM="PA";
static char TH_label[] PROGMEM="TH";
static char BD_label[] PROGMEM="BD";
static char MD_label[] PROGMEM="MD";
static char FV_label[] PROGMEM="FV";
static char HV_label[] PROGMEM="HV";
static char RS_label[] PROGMEM="RS";
static char CF_label[] PROGMEM="CF";
static char SI_label[] PROGMEM="SI";
static char O0_label[] PROGMEM="O0";

// Supported commands, Commands and parameters in ASCII
// Example: ATDA14 means: change the RF module Destination Address to 14

typedef int (*AT_Command_Function_t)(); 

typedef struct
{
  const char *name;
  AT_Command_Function_t function;
}  AT_Command_t;


static AT_Command_t atCommands[] PROGMEM =
{
// Addressing:
  AT_COMMAND (DA), // Destination address              (0~255)
  AT_COMMAND (MA), // My address                       (0~255)
  AT_COMMAND (AC), // address check option             (0: no, 1: address check , 2: address check and 0 broadcast )
// RF
  AT_COMMAND (PA), // Power amplifier                  (0: -30 , 1: -20 , 2: -15 , 3: -10 , 4: 0 , 5: 5 , 6: 7 , 7: 10 )
  AT_COMMAND (CF), // select CCx configuration        (0: 915 Mhz - 76.8k, 1: 915 Mhz - 1.2k, 2: 868 Mhz - 76.8k, 3: 868 Mhz - 1.2k, 4: 433 Mhz)
  AT_COMMAND (SI), // enable the return of status info bytes RSSI and LQI (1:enable,0:disable). -Added by Icing
// Serial
  AT_COMMAND (BD), // Uart baudrate                    (0: 9600 , 1:19200, 2:38400 ,3:115200)
  AT_COMMAND (TH), // TH- threshold of transmitting    (0~32) 
// Mode 
  AT_COMMAND (MD), // CCX Working mode                 (0:idle , 1:transmit , 2:receive, 3:transceive,4:sleep)
  AT_COMMAND (O0), // go back to online mode
// Diagnostics:
  AT_COMMAND (FV), // firmware version
  AT_COMMAND (HV), // hardware version
// Miscelaneous
  AT_COMMAND (RS), // restore default settings
};


long baudRateTable[] PROGMEM= {9600,19200,38400,115200};

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
