/* 
 * File:   uart.h
 * Author: Scott
 *
 * Created on September 15, 2018, 11:51 AM
 */

#ifndef UART_H
#define	UART_H

// INCLUDES
#include <stdbool.h>
#include <stdint.h>

// FUNCTION PROTOS
void init_uart();
void init_md_uart();
void disable_md_rxint();
void terminate_rf_uart();
void terminate_md_uart();
void send_uart_ack();
void rf_uart_send(uint8_t data);
void md_uart_send(unsigned char data);
uint8_t md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOk, bool waitToSendMoreData);
uint8_t parse_creg_ok();
uint8_t parse_cereg_ok();
uint8_t parse_csq_ok();

uint8_t mdRxBuf[160];                           // allocate bytes ahead of time


uint8_t numUartRetries = 15;

#endif	/* UART_H */

