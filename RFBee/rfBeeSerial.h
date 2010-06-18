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
#include "rfBeeCore.h"
#include <avr/pgmspace.h>

#define BUFFLEN CCx_PACKT_LEN
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
int OF_command();
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
static char OF_label[] PROGMEM="OF";
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
  AT_COMMAND (CF), // select CCx configuration         (0: 915 Mhz - 76.8k, 1: 915 Mhz - 1.2k, 2: 868 Mhz - 76.8k, 3: 868 Mhz - 1.2k, 4: 433 Mhz)
// Serial
  AT_COMMAND (BD), // Uart baudrate                    (0: 9600 , 1:19200, 2:38400 ,3:115200)
  AT_COMMAND (TH), // TH- threshold of transmitting    (0~32) 
  AT_COMMAND (OF), // Output Format                    (0: payload only, 1: source, dest, payload ,  3: payload len, source, dest, payload, rssi, lqi )
// Mode 
  AT_COMMAND (MD), // CCX Working mode                 (0:idle , 1:transmit , 2:receive, 3:transceive,4:sleep)
  AT_COMMAND (O0), // go back to online mode
// Diagnostics:
  AT_COMMAND (FV), // firmware version
  AT_COMMAND (HV), // hardware version
// Miscelaneous
  AT_COMMAND (RS), // restore default settings
};

// error codes and labels
byte errNo;

static char error_0[] PROGMEM="error: no error";
static char error_1[] PROGMEM="error: received invalid RF data size";
static char error_2[] PROGMEM="error: received invalid RF data";


static char *error_codes[] PROGMEM={
  error_0,
  error_1,
  error_2,
};


long baudRateTable[] PROGMEM= {9600,19200,38400,115200};

// operating mode, see ATMD

byte rfBeeMode;
#define IDLE_MODE 0
#define TRANSMIT_MODE 1     
#define RECEIVE_MODE 2 
#define TRANSCEIVE_MODE 3
#define SLEEP_MODE 4  

#endif
