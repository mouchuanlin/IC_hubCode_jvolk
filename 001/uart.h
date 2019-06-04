/* 
 * File:   uart.h
 * Author: Jennifer
 *
 * Created on April 4, 2018, 5:45 PM
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
bool init_uart();
bool init_uart_modem();
void md_uart_disable();
void rx_uart_disable();
bool md_uart_write_str_ok(unsigned char *data, uint8_t len, bool want_ok);
void md_uart_send(uint8_t data);
unsigned char md_uart_data_ready();
uint8_t md_uart_read_str();
uint8_t md_uart_read();
uint8_t wait_ok(uint8_t count);
bool str_matches(uint8_t raw[], uint8_t expected[], uint8_t len);


/*****************************************************
 * VARIABLES
 ****************************************************/
unsigned int delayBetweenBytes = 10;      // this delay is used to wait for uart byte reception, multiplied by 5ms


#endif	/* UART_H */

