
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
    
//    TXSTA1bits.TRMT = 1;        // Transmit Shift Register status (1 means TSR empty)
    RCSTA1bits.CREN = 1;        // Enable receiver of UART module 1
    TXSTA1bits.TXEN = 1;        // Enable Transmit on UART module 1 
    
//    MD_POWER = 1;
//    BAUDCON1bits.ABDOVF = 1;    // Auto-Baud Detect Overflow status
//    BAUDCONbits.RCIDL = 1;      // Receive Idle Flag bit

//    INTCONbits.GIE = 0;         // Disable before modification
//    PIE1bits.RC1IE = 1;         // Enable Rx interrupt from modem
    PIR1bits.RC1IF = 0;         // Clear Rx flag
//    INTCONbits.GIE = 1;         // Re-enable after modification
//    
}


void enable_md_rxint()
{
    GIE = 0;
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    INTCONbits.PEIE = 1;
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



bool md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOk, bool waitToSendMoreData)
{
    uint8_t index = 0, pos = 0, currentChar, numRetries = 15;
    tmr5_cnt = 0;
    receivedOK = false;
    receivedERR = false;
    timedOut = false;
    while (numRetries-- > 0 && !receivedOK)
    {
        CREN1 = 0;
        timedOut = false;
        for (index = 0; index < len1; index++)
        {
            referenceBuf[index] = *data;
            currentChar = *data;
            md_uart_send(currentChar);
            data++;
        }
    
        // Now time to receive
//        reload_tmr5_pt25s();
        reload_tmr3_10ms();
        CREN1 = 1;
        
        // Look at Green LED pin to check timing i.e. between when it turns on RCIE and when it times out
        G_ON();
        if (waitToSendMoreData)
        {
            while (!RC1IF && !timedOut);
            if (!timedOut)
            {
                disable_tmr3();
                currentChar = RC1REG;
                if (currentChar == '<')
                {
                    return true;
                }
            }
        }
        
        if (wantOk && !waitToSendMoreData)// && md_state == CONFIG)       // OK will come immediately in these states
        {
//            reload_tmr5_pt25s();
            while ((!receivedOK || !receivedERR) && !timedOut)
            {
                while (!RC1IF && !timedOut);
                if (!timedOut)
                {
                    currentChar = RC1REG;
                    mdRxBuf[pos] = currentChar;
                    if (pos >= 2)
                    {
                        if (mdRxBuf[(uint8_t)(pos - 2)] == 'O' && mdRxBuf[(uint8_t)(pos - 1)] == 'K' && \
                            (mdRxBuf[pos] == '\r' || mdRxBuf[pos] == '\n'))
                        {
                            CLRWDT();
//                            disable_tmr5();
                            disable_tmr3();
                            CREN1 = 0;
                            receivedOK = true;
                            receivedERR = false;
                            return receivedOK;
                        }
                        if (pos >= 5)
                        {
                            if (mdRxBuf[(uint8_t)(pos - 5)] == 'E' && mdRxBuf[(uint8_t)(pos - 4)] == 'R' && \
                                    mdRxBuf[(uint8_t)(pos - 3)] == 'R' && mdRxBuf[(uint8_t)(pos - 2)] == 'O' && \
                                    mdRxBuf[(uint8_t)(pos - 1)] == 'R')// && (mdRxBuf[pos] == '\r' || mdRxBuf[pos] == '\n'))
                            {
                                CLRWDT();
//                                disable_tmr5();
                                disable_tmr3();
                                CREN1 = 0;
                                receivedOK = false;
                                receivedERR = true;
                            }
                        }
                    }
                    pos++;
                }
            }
//            INTCONbits.GIE = 0;
//            RCSTA1bits.CREN1 = 1;
//            PIE1bits.RC1IE = 1;
//            PIR1bits.RC1IF = 0;
//            INTCONbits.PEIE = 1;
//            INTCONbits.GIE = 1;
        }
        if (timedOut)
        {
//            disable_tmr5();
            disable_tmr3();
            G_OFF();
        }
//        else if (!wantOk)
//        {
//            receivedOK = true;          // for now
//            enable_md_rxint();
//        }
//        __delay_ms(500);
    }
    if (numRetries == 0)
    {
//        disable_tmr5();
        disable_tmr3();
        return false;
    }
    
    if (!wantOk && !waitToSendMoreData)
    {
        receivedOK = false;     // this variable will be set when the OK is received by interrupt.
        GIE = 0;
        CREN1 = 1;
        RC1IE = 1;
        PEIE = 1;
        RC1IF = 0;
        GIE = 1;
        return true;
    }
    return receivedOK;
}

void md_uart_send(unsigned char data)
{
//    reload_tmr5_pt25s();
    while (!TRMT1);// && !timedOut);
    TX1REG = data;
}