//  rfBeeSerial.h serial interface to rfBee
//  see www.seeedstudio.com for details and ordering rfBee hardware.

//  Copyright (c) 2010 Hans Klunder <hans.klunder (at) bigfoot.com>
//  Author: Hans Klunder, based on the original Rfbee v1.0 firmware by Seeedstudio
//  Version: July 16, 2010
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

int setMyAddress();
int setAddressCheck();
int setPowerAmplifier();
int setCCxConfig();
int changeUartBaudRate();
int setSerialDataMode();
int setRFBeeMode();
int showFirmwareVersion();
int showHardwareVersion();
int resetConfig();
int setSleepMode();


byte serialData[BUFFLEN+1]; // 1 extra so we can easily add a /0 when doing a debug print ;-)
byte serialMode;
volatile int sleepCounter;


// RFbee AT commands

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
static char SL_label[] PROGMEM="SL";

// Supported commands, Commands and parameters in ASCII
// Example: ATDA14 means: change the RF module Destination Address to 14

typedef int (*AT_Command_Function_t)(); 

typedef struct
{
  const char *name;
  const byte configItem;   // the ID used in the EEPROM
  const byte paramDigits;  // how many digits for the parameter
  const byte maxValue;     // maximum value of the parameter
  const byte postProcess;  // do we need to call the function to perform extra actions on change
  AT_Command_Function_t function; // the function which does the real work on change
}  AT_Command_t;


static AT_Command_t atCommands[] PROGMEM =
{
// Addressing:
  { DA_label, CONFIG_DEST_ADDR, 3 , 255, false, 0 },             // Destination address   (0~255)
  { MA_label, CONFIG_MY_ADDR, 3 , 255, true, setMyAddress },     // My address            (0~255)
  { AC_label, CONFIG_ADDR_CHECK, 1 , 2, true, setAddressCheck }, // address check option  (0: no, 1: address check , 2: address check and 0 broadcast )
// RF
  { PA_label, CONFIG_PAINDEX, 1 , 7, true, setPowerAmplifier },  // Power amplifier           (0: -30 , 1: -20 , 2: -15 , 3: -10 , 4: 0 , 5: 5 , 6: 7 , 7: 10 )
  { CF_label, CONFIG_CONFIG_ID, 1 , 5, true, setCCxConfig },     // select CCx configuration  (0: 915 Mhz - 76.8k, 1: 915 Mhz - 1.2k, 2: 868 Mhz - 76.8k, 3: 868 Mhz - 1.2k, 4: 433 Mhz)
// Serial
  { BD_label, CONFIG_BDINDEX, 1 , 3, true, changeUartBaudRate },  // Uart baudrate                    (0: 9600 , 1:19200, 2:38400 ,3:115200)
  { TH_label, CONFIG_TX_THRESHOLD, 2 , 32, false, 0 },            // TH- threshold of transmitting    (0~32) 
  { OF_label, CONFIG_OUTPUT_FORMAT, 1 , 3 , false, 0 },           // Output Format                    (0: payload only, 1: source, dest, payload ,  2: payload len, source, dest, payload, rssi, lqi, 3: same as 2, but all except for payload as decimal and separated by comma's )
// Mode 
  { MD_label, CONFIG_RFBEE_MODE, 1 , 4 , true, setRFBeeMode},    // CCx Working mode                 (0:idle , 1:transmit , 2:receive, 3:transceive,4:lowpower)
  { O0_label, 0, 0 , 0 , true, setSerialDataMode },              // thats o+ zero, go back to online mode
  { SL_label, 0, 0 , 0 , true, setSleepMode },                   // put the rfBee to sleep
// Diagnostics:
  { FV_label, 0, 0 , 0 , true, showFirmwareVersion },           // firmware version
  { HV_label, 0, 0 , 0 , true, showHardwareVersion },           // hardware version
// Miscelaneous
  { RS_label, 0, 0 , 0 , true, resetConfig }                    // restore default settings
};

// error codes and labels
byte errNo;

static char error_0[] PROGMEM="no error";
static char error_1[] PROGMEM="received invalid RF data size";
static char error_2[] PROGMEM="received invalid RF data";
static char error_3[] PROGMEM="RX buffer overflow";
static char error_4[] PROGMEM="CRC check failed";

static char *error_codes[] PROGMEM={
  error_0,
  error_1,
  error_2,
  error_3,
};


long baudRateTable[] PROGMEM= {9600,19200,38400,115200};

// operating modes, used by ATMD

#define IDLE_MODE 0
#define TRANSMIT_MODE 1     
#define RECEIVE_MODE 2 
#define TRANSCEIVE_MODE 3
#define LOWPOWER_MODE 4
#define SLEEP_MODE 5  

#ifdef INTERRUPT_RECEIVE
volatile enum state

#endif

#endif
