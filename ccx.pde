
#include <avr/io.h>
#include "CCx.h"
#include "Atmegax.h"
#include "CcxCfg.h"

//power on reset as discribed in  27.1 of cc1100 datasheet
void CCxPowerOnStartUp()
{
	unsigned char x;
        //while(1)
        {
	SET_SPICS_HIGH();
        delayMicroseconds(1);
	   
	SET_SPICS_LOW();
        delayMicroseconds(10);
	   
	SET_SPICS_HIGH();
        delayMicroseconds(41);
	   
	SET_SPICS_LOW();
        
        
        //delay(1);
	while(READ_SPIDI());

	#ifdef DEBUG                                                                                                                                                                                                         
       Serial.print("send SRES command\r\n");
	#endif
	
       SPDR = CCx_SRES;
	while(!(SPSR & (1<<SPIF)));
	x = SPDR; 

	#ifdef DEBUG
       Serial.print("CCx state:0x");
       Serial.print((long)x,HEX);
       Serial.print("\r\n");
	#endif
	
	while(READ_SPIDI());
	
	SET_SPICS_HIGH();
        }
}

uchar CCxRead(uchar addr)
{
	uchar x;
	SET_SPICS_LOW();

	while(READ_SPIDI());

	SPDR = (addr | 0x80);
  	while(!(SPSR & (1<<SPIF)));
    	x = SPDR; 
    	SPDR = 0;
    	while(!(SPSR & (1<<SPIF)));
    	x = SPDR;
		
    	#ifdef DEBUG_READ
	Serial.print("read:0x");
    	Serial.print(x,HEX);
    	Serial.print("\r\n");
	#endif
	
   	SET_SPICS_HIGH();

	return x;

}

void CCxReadBurst(uchar addr, uchar* dataPtr, uint dataCount)
{
	uchar x;

	SET_SPICS_LOW();

	while(READ_SPIDI());

    	SPDR = (addr | 0xc0);
	while(!(SPSR & (1<<SPIF)));
	x = SPDR; 
	
	#ifdef DEBUG_
       Serial.print("0x");
       Serial.print((long)x,HEX);
       Serial.print("\r\n");
       #endif
	   
	while(dataCount)
	{
	 	SPDR = 0;
		while(!(SPSR & (1<<SPIF)));

	    	*dataPtr++ = SPDR;
		dataCount--;
	}

	SET_SPICS_HIGH();
}

uchar CCxWrite(uchar addr, uchar dat)
{
    	uchar x;
	SET_SPICS_LOW();
	
	while(READ_SPIDI());

	SPDR = addr;
       
	while(!(SPSR & (1<<SPIF)));
	x = SPDR;
	
	SPDR = dat;
	while(!(SPSR & (1<<SPIF)));
    	x = SPDR;
    
    
    	SET_SPICS_HIGH();

    	return x;
}

void CCxWriteBurst(uchar addr, const uchar* dataPtr, uint dataCount)
{
	uchar x;

	SET_SPICS_LOW();

	while(READ_SPIDI());

	SPDR = addr | 0x40;
	while(!(SPSR & (1<<SPIF)));
	x = SPDR;

	while(dataCount)
	{
		SPDR = *dataPtr++;
		while(!(SPSR & (1<<SPIF)));

		dataCount--;
	}

	SET_SPICS_HIGH();
}

uchar CCxStrobe(uchar addr)
{
	uchar x;
	SET_SPICS_LOW();

	while(READ_SPIDI());

	SPDR = addr;
	while(!(SPSR & (1<<SPIF)));
    	x = SPDR;
    
	SET_SPICS_HIGH();

	#ifdef DEBUG_STROB
	Serial.print("0x");
    	Serial.print((long)x,HEX);
    	Serial.print("\r\n");
	#endif
	
    	return x;
}

//configure registers of cc1100 making it work in specific mode
void CCxSetup(const RF_SETTINGS* settings)
{
	// Write register settings
    	CCxWrite(CCx_FSCTRL1,  settings->FSCTRL1);
    	CCxWrite(CCx_FSCTRL0,  settings->FSCTRL0);
    	CCxWrite(CCx_FREQ2,    settings->FREQ2);
    	CCxWrite(CCx_FREQ1,    settings->FREQ1);
    	CCxWrite(CCx_FREQ0,    settings->FREQ0);
    	CCxWrite(CCx_MDMCFG4,  settings->MDMCFG4);
    	CCxWrite(CCx_MDMCFG3,  settings->MDMCFG3);
   	CCxWrite(CCx_MDMCFG2,  settings->MDMCFG2);
  	CCxWrite(CCx_MDMCFG1,  settings->MDMCFG1);
  	CCxWrite(CCx_MDMCFG0,  settings->MDMCFG0);
    	CCxWrite(CCx_CHANNR,   settings->CHANNR);
    	CCxWrite(CCx_DEVIATN,  settings->DEVIATN);
    	CCxWrite(CCx_FREND1,   settings->FREND1);
    	CCxWrite(CCx_FREND0,   settings->FREND0);
    	CCxWrite(CCx_MCSM0 ,   settings->MCSM0 );
    	CCxWrite(CCx_FOCCFG,   settings->FOCCFG);
    	CCxWrite(CCx_BSCFG,    settings->BSCFG);
    	CCxWrite(CCx_AGCCTRL2, settings->AGCCTRL2);
	CCxWrite(CCx_AGCCTRL1, settings->AGCCTRL1);
    	CCxWrite(CCx_AGCCTRL0, settings->AGCCTRL0);
    	CCxWrite(CCx_FSCAL3,   settings->FSCAL3);
    	CCxWrite(CCx_FSCAL2,   settings->FSCAL2);
	CCxWrite(CCx_FSCAL1,   settings->FSCAL1);
    	CCxWrite(CCx_FSCAL0,   settings->FSCAL0);
    	CCxWrite(CCx_FSTEST,   settings->FSTEST);
 	CCxWrite(CCx_TEST2,    settings->TEST2);
    	CCxWrite(CCx_TEST1,    settings->TEST1);
    	CCxWrite(CCx_TEST0,    settings->TEST0);
    	CCxWrite(CCx_FIFOTHR ,settings->FIFOTHR);
	CCxWrite(CCx_IOCFG2,   settings->IOCFG2);
    	CCxWrite(CCx_IOCFG0,   settings->IOCFG0);    
    	CCxWrite(CCx_PKTCTRL1, settings->PKTCTRL1);
    	CCxWrite(CCx_PKTCTRL0, settings->PKTCTRL0);
    	CCxWrite(CCx_ADDR,     settings->ADDR);
    	CCxWrite(CCx_PKTLEN,   settings->PKTLEN);
		
}

uchar CCxReadSetup()
{
        uchar x = 0;
        x = CCxRead(CCx_FSCTRL1);
        x = CCxRead(CCx_FSCTRL0);
        x = CCxRead(CCx_FREQ2);
        x = CCxRead(CCx_FREQ1);
        x = CCxRead(CCx_FREQ0);
        x = CCxRead(CCx_MDMCFG4);
        x = CCxRead(CCx_MDMCFG3);
        x = CCxRead(CCx_MDMCFG2);
        x = CCxRead(CCx_MDMCFG1);
        x = CCxRead(CCx_MDMCFG0);
        x = CCxRead(CCx_CHANNR);
        x = CCxRead(CCx_DEVIATN);
        x = CCxRead(CCx_FREND1);
        x = CCxRead(CCx_FREND0);
        x = CCxRead(CCx_MCSM0);
        x = CCxRead(CCx_FOCCFG);
        x = CCxRead(CCx_BSCFG);
        x = CCxRead(CCx_AGCCTRL2);
        x = CCxRead(CCx_AGCCTRL1);
        x = CCxRead(CCx_AGCCTRL0);
        x = CCxRead(CCx_FSCAL3);
        x = CCxRead(CCx_FSCAL2);
        x = CCxRead(CCx_FSCAL1);
        x = CCxRead(CCx_FSCAL0);
        x = CCxRead(CCx_FSTEST);
        x = CCxRead(CCx_TEST2);
        x = CCxRead(CCx_TEST1);
        x = CCxRead(CCx_TEST0);
     
        x = CCxRead(CCx_FIFOTHR);
     
        x = CCxRead(CCx_IOCFG2);
        x = CCxRead(CCx_IOCFG0);    
        x = CCxRead(CCx_PKTCTRL1);
        x = CCxRead(CCx_PKTCTRL0);
        x = CCxRead(CCx_ADDR);
        x = CCxRead(CCx_PKTLEN);
        return x;
}
