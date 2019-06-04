/* 
 * File:   eeprom.h
 * Author: JV
 *
 * Created on May 4, 2018, 12:22 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
uint8_t ee_read(uint8_t page, uint8_t addr);
void ee_write(uint8_t page, uint8_t addr, uint8_t data);

#endif	/* EEPROM_H */

