/*
 * File: uart.c
 * Author: JV
 * 
 * Date Created: 04 May 2018, 11:39 AM
 * Last Modified: 04 May 2018, 11:39 AM
 */

#include "config.h"
#include "uart.h"

void init_uart_md()
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
    
    RCSTA1bits.CREN = 1;        // Enable receiver of UART module 1
    TXSTA1bits.TXEN = 1;        // Enable Transmit on UART module 1 
    
    MD_POWER = 1;
    PIR1bits.RC1IF = 0;         // Clear Rx flag
}


void setup_timer3()
{
    T3CON = 0x71;
    TMR3H = 0x20;   //0x40 => 50ms
    TMR3L = 0;
}


bool md_uart_send_ok(uint8_t *toSend, uint8_t lengthToSend, uint8_t *rtnStr, bool want_ok, uint8_t wait_count)
{
    uint8_t buf[MAX_RTN_STRING_LEN],temp, i = 0, count = wait_count;
    md_uart_write(toSend, lengthToSend);
    
    /* If the calling function requires an "OK" for verification,
     * it can use YES for the want_ok parameter to store in the return buffer. 
     * This is not desired for other commands where user wants direct access to
     * the return data flow.
     * For the latter, other manual implementations must be asserted.
     */
    if (!want_ok)
        return true;
    
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
                
                    if (buf[(uint8_t)(i-2)] == 'O' && buf[(uint8_t)(i-1)] == 'K' && (buf[(uint8_t)(i-3)] == '\n' || buf[(uint8_t)(i-3)] == '\r'))
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        for (uint8_t k = 0; k <= i; k ++)
                        {
                            // Need to verify pointer handling is correct
                            *rtnStr = buf[k];             // write all valid data into rtnStr buffer
                            rtnStr++;
                        }
                        return true;        // "OK" received - success
                    }
                    else if (buf[(uint8_t)(i-3)] == 'E' && buf[(uint8_t)(i-2)] == 'R' && buf[(uint8_t)(i-1)] == 'R' \
                            && (buf[(uint8_t)(i-4)] == '\n' || buf[(uint8_t)(i-4)] == '\r'))
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        return false;       // Error received
                    }
                
			}
		}
		TMR3IF = 0;
	}
    TMR3ON = 0;
    RC1IE = 1;
    CREN1 = 0;
  	return false;       // no valid response received
}

void md_uart_write(uint8_t *data, uint8_t len)
{
    uint8_t index, currentChar;
    index = 0;
    CREN1 = 0;
    for (index; index < len; index++)
    {
        currentChar = *data;
        md_uart_transmit(currentChar);
        data++;
    }
}

void md_uart_transmit(unsigned char data)
{
    while (!TRMT1);
    TX1REG = data;
}


bool str_matches(uint8_t *raw, uint8_t *expected, uint8_t len)
{
    // First lay the groundwork and find point of reference
    uint8_t matchCount = 0;
    for (uint8_t i = 0; i < MAX_RTN_STRING_LEN; i ++)
    {
        // If the chars match
        if (*raw == *expected && matchCount < len-1)
        {
            matchCount++;
            expected++;
        }
        // If they don't match but it looked like they had until now
        else if (*raw != *expected && matchCount > 0)
        {
            // Reset points of reference in strings
            raw -= matchCount;
            expected -= matchCount;
            matchCount = 0;
        }
        // If we've reached the maximum length that we want to match
        if (matchCount == len-1)
            return true;
        raw++;
    }
    // If we reach the end of the raw string and find no match
    return false;
}

