#ifndef TESTIO_H
#define TESTIO_H

#include "globals.h"
#include "config.h"

#define HARDWAREVERSION 11  // 1.1 , version number needs to fit in byte (0~255) to be able to store it into config, this should only be changed by the manufacturer !

#define IO_PD_4 4
#define IO_PC_4 18

#define IO_PB_1 9
#define IO_PC_5 19

#define IO_PB_0 8
#define IO_PD_6 6

#define IO_PD_7 7
#define IO_PD_5 5

#define IO_PC_0 14
#define IO_PC_2 16


#define IO_PC_1 15
#define IO_ADC_7 7
#define IO_PC_3 17

int TestIO();

#endif

