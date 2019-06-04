
#include "config.h"
#include "modem.h"
#include "uart.h"



bool toggle = true;



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
    
//    TXSTA1bits.TRMT = 1;        // Transmit Shift Register status (1 means TSR empty)
    RCSTA1bits.CREN = 0;        // Disable receiver of UART module 1
    TXSTA1bits.TXEN = 1;        // Enable Transmit on UART module 1 
    
//    MD_POWER = 1;
//    BAUDCON1bits.ABDOVF = 1;    // Auto-Baud Detect Overflow status
//    BAUDCONbits.RCIDL = 1;      // Receive Idle Flag bit

//    INTCONbits.GIE = 0;         // Disable before modification
//    PIE1bits.RC1IE = 1;         // Enable Rx interrupt from modem
//    PIR1bits.RC1IF = 0;         // Clear Rx flag
//    INTCONbits.GIE = 1;         // Re-enable after modification
//    
}


void enable_md_rxint()
{
    GIE = 0;
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    INTCONbits.PEIE = 1;
    CREN1 = 1;
    endPtr = 0;
    GIE = 1;
}

void disable_md_rxint()
{
    CREN1 = 0;
    GIE = 0;
    PIE1bits.RC1IE = 0;
    GIE = 1;
}


void terminate_md_uart()
{
    RCSTA1bits.SPEN = 0;
//    TXEN1 = 0;
//    RCSTA1 = 0x00;
    TX_MD_TRIS = OUTPUT;
    TX_MD = 0;
    RC1IE = 0;
}



uint8_t md_uart_write_str_ok(unsigned char *data, uint8_t len, \
        bool wantOK, bool waitToSendMoreData)       // wantOK and waitToSendMore should only be 'NO' when waiting to send termination
{                                                   // character (ctrl-z)
    uint8_t currentChar, temp;
    switch (atCmdState)
    {
        case AT_SEND:
            tmr3_cnt = 0;
            endPtr = 0;
            receivedOK = false;
            receivedERR = false;
            if (numRetries -- > 0)
            {
                init_md_uart();
                __delay_ms(10);
                timedOut = false;
                /* SEND */
                for (uint8_t index = 0; index < len; index++)
                {
                    currentChar = *data;
                    md_uart_send(currentChar);
                    data++;
                }
                if (wantOK && !waitToSendMoreData)
                {
                    enable_md_rxint();
                    reload_tmr3_1s();
                    atCmdState = AT_WAIT_OK;
                }
                else if (!wantOK && !waitToSendMoreData)
                {
                    terminate_md_uart();
                    return true;
                }
//                else if (wantOKImmediate)       // waits for 50ms in this loop, then moves on to use longer timer if nothing received
//                {
//                    CREN1 = 1;
//                    T3CON = 0b00010101;     // 1:2 prescale on Fosc/4, clk src is internal instruction cycle,
//                                            // get value by two 8-bit reads, timer3 ON
//                    TMR3H = 0x3C;           // Set to 50ms interval
//                    TMR3L = 0xAF;
//                    endPtr = 0;
//                    while (!receivedOK && !receivedERR && !timedOut && !TMR3IF)
//                    {
//                        while (!RC1IF && !TMR3IF);
//                        currentChar = RC1REG;
//                        mdRxBuf[endPtr++] = currentChar;
//                        if (endPtr >= sizeof (mdRxBuf))
//                            endPtr--;
//                        if (endPtr > 2) {
//                            // Check for OK or ERROR
//                            if (mdRxBuf[(uint8_t) (endPtr - 3)] == 'O' && mdRxBuf[(uint8_t) (endPtr - 2)] == 'K' \
//                                     && (mdRxBuf[(uint8_t)(endPtr - 1)] == '\r' || mdRxBuf[(uint8_t)(endPtr - 1)] == '\n')) {
//                                tmr3_cnt = 0;
//                                disable_tmr3(); // Turn off timer & disable associated interrupt
//                                receivedOK = true;
//                                receivedERR = false;
//                                timedOut = false;
//                                disable_md_rxint();
//                            } else if (endPtr >= 5 && mdRxBuf[(uint8_t) (endPtr - 5)] == 'E' && \
//                                     mdRxBuf[(uint8_t) (endPtr - 4)] == 'R' && mdRxBuf[(uint8_t) (endPtr - 3)] == 'R' && \
//                                     mdRxBuf[(uint8_t) (endPtr - 2)] == 'O' && mdRxBuf[endPtr - 1] == 'R') {
//                                //                tmr3_cnt = 0;
////                                disable_tmr3();             // Don't turn off timer or disable associated interrupt yet
//                                receivedOK = false;
//                                receivedERR = true;
//                                timedOut = false;
//                                disable_md_rxint();
//                            }
//                        } 
//                        if (currentChar == '>') {
//                            tmr3_cnt = 0;
//                            disable_tmr3();
//                            disable_md_rxint();
//                            receivedOK = false;
//                            receivedERR = false;
//                            receivedCarat = true;
//                            atCmdState = AT_WAIT_MORE;
//                        }
//                    }
//                    if (TMR3IF)
//                    {
//                        enable_md_rxint();
//                        reload_tmr3_10ms();
//                        atCmdState = AT_WAIT_OK;
//                    }
//                    else
//                    {
//                        atCmdState = AT_WAIT_OK;
//                        disable_tmr3();
//                    }
//                }
                else if (waitToSendMoreData)
                {
                    enable_md_rxint();
                    reload_tmr3_1s();
                    atCmdState = AT_WAIT_MORE;
                }
            }
            else
            {
                numRetries = 15;
                return ERR_CODE_RETRIES_ENDED;
            }
            break;
        case AT_WAIT_MORE:
            if (receivedCarat)
            {
                disable_tmr3();
                terminate_md_uart();
                receivedCarat = false;
                atCmdState = AT_SEND;
                return ERR_CODE_REC_OK;
            }
            else if (!receivedCarat && timedOut && numRetries > 0)
            {
                disable_tmr3();
                terminate_md_uart();
                timedOut = false;
                atCmdState = AT_SEND;
                return ERR_CODE_TIMED_OUT;
            }
            if (numRetries == 0)
            {
                disable_tmr3();
                terminate_md_uart();
                atCmdState = AT_SEND;
                return ERR_CODE_RETRIES_ENDED;
            }
            break;
        case AT_WAIT_OK:
            if (receivedOK && !receivedERR)// && tmr3_cnt > _T3_100ms)// && !timedOut)
            {
                reset_retry_count();
                tmr3_cnt = 0;
                disable_tmr3();
                terminate_md_uart();
                receivedOK = false;
                atCmdState = AT_SEND;
                return ERR_CODE_REC_OK;
            }
            else if (!receivedOK && receivedERR && timedOut)        // if received ERR, wait for time out
            {
                receivedERR = false;
                timedOut = false;
                disable_tmr3();
                terminate_md_uart();
                atCmdState = AT_SEND;
                return ERR_CODE_REC_ERROR;
            }
            else if (!receivedOK && !receivedERR && timedOut)
            {
                toggle = ~toggle;
                G_LED = toggle;
                timedOut = false;
                disable_tmr3();
                terminate_md_uart();
                atCmdState = AT_SEND;
                return ERR_CODE_TIMED_OUT;
            }
            if (numRetries == 0)
            {
                /* reset numRetries here */
                disable_tmr3();
                terminate_md_uart();
                atCmdState = AT_SEND;
                return ERR_CODE_RETRIES_ENDED;
            }
    }
//    atCmdState = AT_SEND;
    if (receivedOK)
        return ERR_CODE_REC_OK;
    else if (receivedERR)
        return ERR_CODE_REC_ERROR;
    else if (timedOut)
        return ERR_CODE_TIMED_OUT;
    else
        return ERR_CODE_NA;
}


//bool md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
//        bool wantOk, bool waitToSendMoreData)
//{
//    uint8_t index = 0, pos = 0, currentChar, numRetries = 15;
//    tmr5_cnt = 0;
//    receivedOK = false;
//    receivedERR = false;
//    timedOut = false;
//    while (numRetries-- > 0 && !receivedOK)
//    {
//        CREN1 = 0;
//        timedOut = false;
//        for (index = 0; index < len1; index++)
//        {
//            referenceBuf[index] = *data;
//            currentChar = *data;
//            md_uart_send(currentChar);
//            data++;
//        }
//    
//        // Now time to receive
////        reload_tmr5_pt25s();
//        reload_tmr3_10ms();
//        CREN1 = 1;
//        
//        // Look at Green LED pin to check timing i.e. between when it turns on RCIE and when it times out
//        G_ON();
//        if (waitToSendMoreData)
//        {
//            while (!RC1IF && !timedOut);
//            if (!timedOut)
//            {
//                disable_tmr3();
//                currentChar = RC1REG;
//                if (currentChar == '<')
//                {
//                    return true;
//                }
//            }
//        }
//        
//        if (wantOk && !waitToSendMoreData)// && md_state == CONFIG)       // OK will come immediately in these states
//        {
////            reload_tmr5_pt25s();
//            while ((!receivedOK || !receivedERR) && !timedOut)
//            {
//                while (!RC1IF && !timedOut);
//                if (!timedOut)
//                {
//                    currentChar = RC1REG;
//                    mdRxBuf[pos] = currentChar;
//                    if (pos >= 2)
//                    {
//                        if (mdRxBuf[(uint8_t)(pos - 2)] == 'O' && mdRxBuf[(uint8_t)(pos - 1)] == 'K' && \
//                            (mdRxBuf[pos] == '\r' || mdRxBuf[pos] == '\n'))
//                        {
//                            CLRWDT();
////                            disable_tmr5();
//                            disable_tmr3();
//                            CREN1 = 0;
//                            receivedOK = true;
//                            receivedERR = false;
//                            return receivedOK;
//                        }
//                        if (pos >= 5)
//                        {
//                            if (mdRxBuf[(uint8_t)(pos - 5)] == 'E' && mdRxBuf[(uint8_t)(pos - 4)] == 'R' && \
//                                    mdRxBuf[(uint8_t)(pos - 3)] == 'R' && mdRxBuf[(uint8_t)(pos - 2)] == 'O' && \
//                                    mdRxBuf[(uint8_t)(pos - 1)] == 'R')// && (mdRxBuf[pos] == '\r' || mdRxBuf[pos] == '\n'))
//                            {
//                                CLRWDT();
////                                disable_tmr5();
//                                disable_tmr3();
//                                CREN1 = 0;
//                                receivedOK = false;
//                                receivedERR = true;
//                            }
//                        }
//                    }
//                    pos++;
//                }
//            }
////            INTCONbits.GIE = 0;
////            RCSTA1bits.CREN1 = 1;
////            PIE1bits.RC1IE = 1;
////            PIR1bits.RC1IF = 0;
////            INTCONbits.PEIE = 1;
////            INTCONbits.GIE = 1;
//        }
//        if (timedOut)
//        {
////            disable_tmr5();
//            disable_tmr3();
//            G_OFF();
//        }
////        else if (!wantOk)
////        {
////            receivedOK = true;          // for now
////            enable_md_rxint();
////        }
////        __delay_ms(500);
//    }
//    if (numRetries == 0)
//    {
////        disable_tmr5();
//        disable_tmr3();
//        return false;
//    }
//    
//    if (!wantOk && !waitToSendMoreData)
//    {
//        receivedOK = false;     // this variable will be set when the OK is received by interrupt.
//        GIE = 0;
//        CREN1 = 1;
//        RC1IE = 1;
//        PEIE = 1;
//        RC1IF = 0;
//        GIE = 1;
//        return true;
//    }
//    return receivedOK;
//}

void md_uart_send(unsigned char data)
{
//    reload_tmr5_pt25s();
    while (!TRMT1);// && !timedOut);
    TX1REG = data;
}