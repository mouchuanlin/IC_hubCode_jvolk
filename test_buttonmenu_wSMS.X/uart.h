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
void init_md_uart();
void disable_md_rxint();
void terminate_md_uart();
void md_uart_send(unsigned char data);
uint8_t md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOk, bool waitToSendMoreData);
uint8_t parse_creg_ok();
uint8_t parse_cereg_ok();
uint8_t parse_csq_ok();
void setup_timer3();
bool str_matches(uint8_t *raw, uint8_t *expected, uint8_t len);


/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t returnStr[MAX_RTN_STRING_LEN];
uint8_t mdRxBuf[160];                           // allocate bytes ahead of time


uint8_t numUartRetries = 15;

#endif	/* UART_H */

