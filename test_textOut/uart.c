/* 
 * File:   uart.c
 * Author: Jennifer
 * 
 * This file contains all functions related to UART operation.
 *
 * Created on April 4, 2018, 5:47 PM
 */

#include "config.h"
#include "module.h"
#include "uart.h"

bool init_uart()
{
    init_uart_modem();
//    init_uart_rx();
    return 1;
}

bool init_uart_modem()
{
    BAUDCON1bits.BRG16 = 1;     // Use 16-bit baud rate generator (SPBRGH1:SPBRG1)
    BAUDCON1bits.RCIDL = 1;
    BAUDCONbits.CKTXP = 0;
    BAUDCONbits.WUE = 0;
    BAUDCONbits.ABDEN = 0;
    TXSTA1bits.BRGH = 1;
    TXSTA1bits.SYNC = 0;        // Asynchronous transmit
    SPBRGH1 = 0;                // To get 115.2kbaud with Fosc = 4MHz, use SPBRGH:SPBRG = 8
    SPBRG1 = 16;

    RC1STAbits.SPEN = 1;        // Enable Serial port
    RX_MD_TRIS = 1;
    TX_MD_TRIS = 0;
    
//    TXSTA1bits.TRMT = 1;        // Transmit Shift Register status (1 means TSR empty)
    RCSTA1bits.CREN = 1;        // Enable receiver of UART module 1
    TXSTA1bits.TXEN = 1;        // Enable Transmit on UART module 1 
    
    MD_POWER = 1;
//    BAUDCON1bits.ABDOVF = 1;    // Auto-Baud Detect Overflow status
//    BAUDCONbits.RCIDL = 1;      // Receive Idle Flag bit

//    INTCONbits.GIE = 0;         // Disable before modification
//    PIE1bits.RC1IE = 1;         // Enable Rx interrupt from modem
    PIR1bits.RC1IF = 0;         // Clear Rx flag
//    INTCONbits.GIE = 1;         // Re-enable after modification
//    
    return true;
}

//init_uart_rx()
//{
//    TXEN2 = 1;                  // Enable Transmit on UART module 2
//    BRGH2 = 1;                  // High speed for Async mode
//    TRMT2 = 1;                  // Transmit Shift Register status (1 means TSR empty)
//    SYNC2 = 0;                  // Async transmit
//    CREN2 = 1;                  // Enable UART module 2 receiver
//    SPEN2 = 1;                  // Enable serial port
//    BAUDCON2bits.BRG16 = 1;
//    SPBRG2 = 207;               // for 19.2kbaud
//    SPBRGH2 = 0;
//    WPUB5 = 1;                  // Enable weak pull-up on interrupt line from receiver
//    RC2IE = 1;                  // Enable interrupt-on-change from receiver
//}

void md_uart_disable()
{
    TXEN1 = 0;
    RCSTA1 = 0x00;
    TX_MD_TRIS = OUTPUT;
    TX_MD = 0;
    RC1IE = 0;
}

void rx_uart_disable()
{
    RCSTA2 = 0x00;
    TXEN2 = 0;
    TX_WIFI_TRIS = OUTPUT;
    TX_WIFI = 0;
    RC2IE = 0;
}

bool md_uart_write_str_ok(unsigned char *data, uint8_t len, bool want_ok)
{
    uint8_t index, currentChar;
    index = 0;
    CREN1 = 0;
    for (index; index < len; index++)
    {
        currentChar = *data;
        md_uart_send(currentChar);
        data++;
    }
    /* If the calling function requires an "OK" for verification,
     * it can use YES for want_ok parameter. This is not desired for
     * commands like AT+CSQ, where the return values are desired.
     * For the latter, other manual implementations must be asserted.
     */
    if (want_ok)
        return (bool)(md_uart_read_str() == 'K');
    else
        return 1;
}

void md_uart_send(unsigned char data)
{
    while (!TRMT1);
    TX1REG = data;
}

unsigned char md_uart_data_ready()
{
  return (unsigned char)(RC1IF);
}

/****** TO DO: ADD FRAMING ERROR, OVERRUN ERROR DETECTION ******/
uint8_t md_uart_read_str()
{
    return wait_ok(20);
}

uint8_t md_uart_read()
{
    while(!md_uart_data_ready());// Wait for data to be received
    return RC1REG;
}


uint8_t wait_ok(uint8_t count) // duration of this wait is determined by 50ms * count
{
    uint8_t buf[40],temp, i = 0;
//    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    while (count-- != 0)
    { 
        setup_timer3();
        while(!TMR3IF)
        {
			if( RC1IF == 1 )
			{                /* Re-think the following check: store in buffer, and then look for the words "ok" and "error"  */
				temp = RC1REG;
                buf[i++] = temp;
                
                if (i >= 20)
                    i = 19;
                
                if (temp == 0x0A)
                {
                    if (buf[0] == 'O' && buf[1] == 'K')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        return ('K');
                    }
                    else if (buf[0] == 'E' && buf[1] == 'R' && buf[2] == 'R')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        return ('E');
                    }
                    i = 0;
                }
			}
		}
		TMR3IF = 0;
	}
    TMR3ON = 0;
    RC1IE = 1;
    CREN1 = 0;
  	return('N');
}


bool str_matches(uint8_t *raw, uint8_t *expected, uint8_t len)
{
    bool matches = true;
    for (uint8_t i = 0; i < len; i ++)
    {
        matches &= (bool)(*raw == *expected);
        raw++;
        expected++;
    }
    
    return matches;
}



bool md_get_str_ok(uint8_t *toSend, uint8_t lengthToSend, uint8_t *rtnStr)
{
    md_uart_write_str_ok(toSend, lengthToSend, NO);
    uint8_t buf[40],temp, i = 0, count = 15;
//    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    while (count-- != 0)
    { 
        setup_timer3();
        while(!TMR3IF)
        {
			if( RC1IF == 1 )
			{                /* Re-think the following check: store in buffer, and then look for the words "ok" and "error"  */
				temp = RC1REG;
                buf[i++] = temp;
                
                if (i >= sizeof(buf))
                    i = sizeof(buf) - 1;
                
                    if (buf[(uint8_t)(i-1)] == 'O' && buf[i] == 'K')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        for (uint8_t k = 0; k <= i; k ++)
                            rtnStr[k] = buf[k];
                        return true;
                    }
                    else if (buf[(uint8_t)(i-2)] == 'E' && buf[(uint8_t)(i-1)] == 'R' && buf[i] == 'R')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        return false;
                    }
                
			}
		}
		TMR3IF = 0;
	}
    TMR3ON = 0;
    RC1IE = 1;
    CREN1 = 0;
  	return false;
}