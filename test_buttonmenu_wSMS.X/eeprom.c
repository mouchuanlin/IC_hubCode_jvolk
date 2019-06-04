/* 
 * File:   eeprom.c
 * Author: JV
 *
 * Created on May 4, 2018, 12:22 PM
 */

#include "eeprom.h"


uint8_t ee_read(uint8_t page, uint8_t addr)
{
    unsigned char data;
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	CFGS = 0;
	EEPGD = 0;
	RD = 1;
    data = EEDATA;
    GIE = 1;
	return (data);
}


void ee_write(uint8_t page, uint8_t addr, uint8_t data)
{
    GIE = 0;
    EEADRH = page;
    EEADR = addr;
	EEDATA = data;
	CFGS = 0;
	EEPGD = 0;
	WREN = 1;
	GIE = 0;
	EECON2 = 0x55;              // Write-order as specified in the datasheet
	EECON2 = 0xAA;
	WR = 1;
    GIE = 1;
	WREN = 0;
	while( WR == 1 );
    PIR2bits.EEIF = 0;          // Clear auto-generated interrupt
    GIE = 1;
}


uint8_t ee_read_ascii_to_value(uint8_t page, uint8_t addr)
{
    uint8_t data;
    data = ee_read(page, addr);
    if (data >= 'A')
        data = (uint8_t)((data - 0x41) + 0x0A);
    else
        data = (uint8_t)(data - 0x30);
    
    return data;
}