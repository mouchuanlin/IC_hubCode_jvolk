/* 
 * File:   uart.h
 * Author: JV
 *
 * Created on May 4, 2018, 11:41 AM
 */

#ifndef UART_H
#define	UART_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"


/*****************************************************
 * DEFINES
 ****************************************************/
#define MAX_RTN_STRING_LEN      100

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void init_uart_md();
bool md_uart_send_ok(uint8_t *toSend, uint8_t lengthToSend, uint8_t *rtnStr, bool want_ok, uint8_t wait_count);
void md_uart_write(uint8_t *data, uint8_t len);
void md_uart_transmit(unsigned char data);
void setup_timer3();
bool str_matches(uint8_t *raw, uint8_t *expected, uint8_t len);


/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t returnStr[MAX_RTN_STRING_LEN];

#endif	/* UART_H */

