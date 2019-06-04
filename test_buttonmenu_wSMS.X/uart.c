/*
 * File: uart.c
 * Author: JV
 * 
 * Date Created: 04 May 2018, 11:39 AM
 * Last Modified: 04 May 2018, 11:39 AM
 */

#include "config.h"
#include "modem.h"
#include "uart.h"

void init_md_uart()
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
    
    MD_PWR = 1;
    PIR1bits.RC1IF = 0;         // Clear Rx flag
}

void enable_md_rxint()
{
    GIE = 0;
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    INTCONbits.PEIE = 1;
    GIE = 1;
}


void setup_timer3()
{
    T3CON = 0x71;
    TMR3H = 0x20;   //0x40 => 50ms
    TMR3L = 0;
}


//bool md_uart_send_ok(uint8_t *toSend, uint8_t lengthToSend, uint8_t *rtnStr, bool want_ok, uint8_t wait_count)
//{
//    uint8_t buf[MAX_RTN_STRING_LEN],temp, i = 0, count = wait_count;
//    md_uart_write(toSend, lengthToSend);
//    
//    /* If the calling function requires an "OK" for verification,
//     * it can use YES for the want_ok parameter to store in the return buffer. 
//     * This is not desired for other commands where user wants direct access to
//     * the return data flow.
//     * For the latter, other manual implementations must be asserted.
//     */
//    if (!want_ok)
//        return true;
//    
//    RC1IE = 0;
//    CREN1 = 1;
//    while (count-- != 0)
//    { 
//        setup_timer3();
//        while(!TMR3IF)
//        {
//			if( RC1IF == 1 )
//			{                /* Re-think the following check: store in buffer, and then look for the words "ok" and "error"  */
//				temp = RC1REG;
//                buf[i++] = temp;
//                
//                if (i >= sizeof(buf))
//                    i = sizeof(buf) - 1;
//                
//                    if (buf[(uint8_t)(i-2)] == 'O' && buf[(uint8_t)(i-1)] == 'K' && (buf[(uint8_t)(i-3)] == '\n' || buf[(uint8_t)(i-3)] == '\r'))
//                    {
//                        RC1IE = 1;
//                        CREN1 = 0;
//                        for (uint8_t k = 0; k <= i; k ++)
//                        {
//                            // Need to verify pointer handling is correct
//                            *rtnStr = buf[k];             // write all valid data into rtnStr buffer
//                            rtnStr++;
//                        }
//                        return true;        // "OK" received - success
//                    }
//                    else if (buf[(uint8_t)(i-3)] == 'E' && buf[(uint8_t)(i-2)] == 'R' && buf[(uint8_t)(i-1)] == 'R' \
//                            && (buf[(uint8_t)(i-4)] == '\n' || buf[(uint8_t)(i-4)] == '\r'))
//                    {
//                        RC1IE = 1;
//                        CREN1 = 0;
//                        return false;       // Error received
//                    }
//                
//			}
//		}
//		TMR3IF = 0;
//	}
//    TMR3ON = 0;
//    RC1IE = 1;
//    CREN1 = 0;
//  	return false;       // no valid response received
//}

void md_uart_write(uint8_t *data, uint8_t len)
{
    uint8_t index, cChar;
    index = 0;
    CREN1 = 0;
    for (index; index < len; index++)
    {
        cChar = *data;
        md_uart_send(cChar);
        data++;
    }
}

void md_uart_send(unsigned char data)
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



uint8_t md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOk, bool waitToSendMoreData)
{
    if (!waiting_uart && !waiting_carrot)
    {
        uint8_t index = 0, pos = 0;
        CREN1 = 0;
        tmr5_cnt = 0;
        receivedOK = false;
        receivedERR = false;
        timedOut = false;
        while (numUartRetries-- > 0 && !receivedOK)
        {
            timedOut = false;
            for (index = 0; index < len1; index++)
            {
                referenceBuf[index] = *data;
                currentChar = *data;
                md_uart_send(currentChar);
                data++;
            }
            if (!waitToSendMoreData && wantOk)
            {
                CREN1 = 1;
                enable_md_rxint();
                reload_tmr5_pt25s();
                waiting_uart = true;
            }
            else if (waitToSendMoreData)
                waiting_carrot = true;
        }
    }
    // Now time to receive
    if (waitToSendMoreData && waiting_carrot)
    {
        CREN1 = 1;
        while (!RC1IF);
        currentChar = RC1REG;           // assume the carat comes fast enough
        if (currentChar == '<')
        {
            waiting_carrot = false;
            return 1;
        }
        else
        {
            waiting_carrot = false;
            return 4;       // error
        }
    }
    if (wantOk && waiting_uart)// && md_state == CONFIG)       // OK will come immediately in these states
    {
        if (receivedOK)
        {
            disable_tmr5();
            CREN1 = 0;
            waiting_uart = false;
            return 1;
        }
        else if (receivedERR)
        {
            disable_tmr5();
            CREN1 = 0;
            waiting_uart = false;
            return 4;
        }
        else if (timedOut)
        {
            disable_tmr5();
            CREN1 = 0;
            waiting_uart = false;
            return 5;
        }
        else        // still waiting
            return 3;
    }
    else if (!wantOk)
    {
        receivedOK = true;          // for now
        return 1;
    }
    
//    if (!wantOk && !waitToSendMoreData)
//    {
//        receivedOK = false;     // this variable will be set when the OK is received by interrupt.
//        GIE = 0;
//        CREN1 = 1;
//        RC1IE = 1;
//        PEIE = 1;
//        RC1IF = 0;
//        GIE = 1;
//        return 1;
//    }
    return 4;
}



// MODEM COMMAND PARSING

// error codes: 1 = OK. 4 = NOT VALID RESPONSE/STILL NEED TO WAIT FOR NETWORK
uint8_t parse_creg_ok()
{
    uint8_t creg_rsp[] = "+CREG: ";
    uint8_t registration_num;
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i++)
    {
        if (mdRxBuf[i] == creg_rsp[0] && mdRxBuf[(uint8_t)(i + 1)] == creg_rsp[1] && \
                mdRxBuf[(uint8_t)(i + 2)] == creg_rsp[2] && mdRxBuf[(uint8_t)(i + 3)] == creg_rsp[3] && \
                mdRxBuf[(uint8_t)(i + 4)] == creg_rsp[4] && mdRxBuf[(uint8_t)(i + 5)] == creg_rsp[5])
        {
            registration_num = mdRxBuf[(uint8_t)(i + 9)];
            if (registration_num > '0' && registration_num < '6')
                return 1;
            else
                return 4;
        }
    }
    return 4;       // got through loop without finding string
}

uint8_t parse_cereg_ok()
{
    uint8_t cereg_rsp[] = "+CEREG: ";
    uint8_t registration_num;
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i++)
    {
        if (mdRxBuf[i] == cereg_rsp[0] && mdRxBuf[(uint8_t)(i + 1)] == cereg_rsp[1] && \
                mdRxBuf[(uint8_t)(i + 2)] == cereg_rsp[2] && mdRxBuf[(uint8_t)(i + 3)] == cereg_rsp[3] && \
                mdRxBuf[(uint8_t)(i + 4)] == cereg_rsp[4] && mdRxBuf[(uint8_t)(i + 5)] == cereg_rsp[5] && \
                mdRxBuf[(uint8_t)(i + 6)] == cereg_rsp[6])
        {
            registration_num = mdRxBuf[(uint8_t)(i + 10)];
            if (registration_num > '0' && registration_num < '9')
                return 1;
            else
                return 4;
        }
    }
    return 4;       // got through loop without finding string
}

uint8_t parse_csq_ok()
{
    uint8_t csq_rsp[] = "+CSQ: ";
    uint8_t colon_index = 0, comma_index = 0;
    uint8_t tens_digit, ones_digit;
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i++)
    {
        if (mdRxBuf[i] == csq_rsp[4])
            colon_index = i;
        if (mdRxBuf[i] == ',')
            comma_index == i;
    }
    if (mdRxBuf[(uint8_t)(colon_index - 4)] == csq_rsp[0] && mdRxBuf[(uint8_t)(colon_index - 3)] == csq_rsp[1] && \
            mdRxBuf[(uint8_t)(colon_index - 2)] == csq_rsp[2] && mdRxBuf[(uint8_t)(colon_index - 1)] == csq_rsp[3])
    {
        if (comma_index - colon_index > 3)      // two digits
        {
            tens_digit = (mdRxBuf[(uint8_t)(comma_index - 2)]);
            if (tens_digit >= 0x41)
                tens_digit = (uint8_t)((tens_digit - 0x41 + 0x0A) * 10);
            else
                tens_digit = (uint8_t)((tens_digit - 0x30) * 10);
            ones_digit = (mdRxBuf[(uint8_t)(comma_index - 1)]);
            if (ones_digit >= 0x41)
                ones_digit -= 0x41;
            else
                ones_digit -= 0x30;
            tens_digit += ones_digit;
            if (tens_digit > 0 && tens_digit < 99)
                return 1;
            else
                return 4;
        }
        else            // one digit
        {
            ones_digit = (uint8_t)(mdRxBuf[(uint8_t)(comma_index - 1)] - 0x30);
            if (ones_digit > 0)
                return 1;
            else
                return 4;
        }
    }
    return 4;       // got through loop without finding string
}
