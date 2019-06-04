/*
 * File: uart.c
 * Author: THE MASTER
 * 
 * Date Created: 17 May 2018, 17:15
 */


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"

/*****************************************************
 * FUNCTION PROTOS
 ****************************************************/



void start_uart()
{
    RX_WIFI_TRIS = 1;           /* Set RX pin as input */
    TX_WIFI_TRIS = 0;           /* Set TX pin as output */
    
    
    TX2STAbits.BRGH = 0;           /* Baud = 9600 */
    SPBRG2 = 207;         /* */
    
    TX2STAbits.SYNC = 0;           /* Config uart as asynchronous */
//    RC2STAbits.SPEN = 1;           /* Enable UART module */
//    
//    /* Enable both Rx and Tx ports */
//    RC2STAbits.CREN = 1;
//    TX2STAbits.TXEN = 1;
    
    TXSTA2 = 0x26;
    RCSTA2 = 0x90;
    BAUDCON2 = 0xc8;
    
}

void enable_RX_uart_interrupt()
{
//    haveHeader = false;
//    crcHighCorrect = false;
//    crcLowCorrect = false;
    GIE = 0;
    RC2IE = 1;
    RC2IF = 0;
    GIE = 1;
}

void disable_RX_uart_interrupt()
{
    RCIE = 0;
}