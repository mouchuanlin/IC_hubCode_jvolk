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
uint8_t read_sms(uint8_t txtAddr1, uint8_t txtAddr2, uint8_t txtAddr3);
void check_sms();
void delete_sms(uint8_t p1, uint8_t p2, uint8_t p3);
bool receive_sms_init();
bool receive_sms();

#endif	/* SMS_H */

