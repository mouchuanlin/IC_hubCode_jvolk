/* 
 * File:   sms.h
 * Author: Jennifer
 *
 * Created on April 11, 2018, 11:01 PM
 */

#ifndef SMS_H
#define	SMS_H

#include "config.h"
#include "module.h"
#include "uart.h"

/*****************************************************
 * VARIABLES
 ****************************************************/
bool send_text(uint8_t *msg, uint8_t len, uint8_t *number);

#endif	/* SMS_H */

