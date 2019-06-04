/* 
 * File:   eeprom.c
 * Author: Jennifer
 *
 * This file contains functions related to EEPROM reads/writes.
 * 
 * Created on April 9, 2018, 4:13 PM
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