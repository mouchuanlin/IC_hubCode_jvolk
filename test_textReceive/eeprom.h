/* 
 * File:   eeprom.h
 * Author: Jennifer
 *
 * Created on April 9, 2018, 4:13 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "config.h"

uint8_t ee_read(uint8_t page, uint8_t addr);
void ee_write(uint8_t page, uint8_t addr, uint8_t data);

#endif	/* EEPROM_H */

