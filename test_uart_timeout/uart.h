/* 
 * File:   uart.h
 * Author: Scott
 *
 * Created on June 12, 2018, 3:37 PM
 */

#ifndef UART_H
#define	UART_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void init_md_uart();
void terminate_md_uart();
bool md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOk, bool waitToSendMoreData);
void md_uart_send(unsigned char data);
void enable_md_rxint();
void disable_md_rxint();

/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t mdRxBuf[160];                           // allocate bytes ahead of time
uint8_t endPtr = 0;
bool receivedOK, receivedERR, receivedRESP, timedOut;

    uint8_t referenceBuf[60];

#endif	/* UART_H */

