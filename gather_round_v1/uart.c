
#include "config.h"
#include "modem.h"
#include "uart.h"



void init_uart()
{
    TRIS_WIFI_TX = 1;
    TRIS_WIFI_RX = 0;
//    TRIS_MD_TX = 1;
//    TRIS_MD_RX = 0;
    TRIS_WIFI_INT = 1;
    
//    TX1STA = 0x26;          //00100110
//    RC1STA = 0x90;          //10010000
//    BAUDCON1 = 0xc8;        //11001000
//    SPBRG1 = 16;            // 115.2k
//    SPBRGH1 = 0;
    
    TX2STA = 0x26;          //00100110
    RC2STA = 0x90;          //10010000
    BAUDCON2 = 0xc8;        //11001000
    SPBRG2 = 207;           //9600bps
    SPBRGH2 = 0;
    WPUB5 = 1;
    RC2IE = 1;
    
    init_md_uart();
}


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
    TRIS_MD_TX = 1;
    TRIS_MD_RX = 0;
    
//    TXSTA1bits.TRMT = 1;        // Transmit Shift Register status (1 means TSR empty)
    RCSTA1bits.CREN = 1;        // Enable receiver of UART module 1
    TXSTA1bits.TXEN = 1;        // Enable Transmit on UART module 1 
    
    PIR1bits.RC1IF = 0;         // Clear Rx flag
}

void terminate_rf_uart()
{
    RCSTA2bits.SPEN = 0;
    RCSTA2bits.CREN = 0;
    TXSTA2bits.TXEN = 0;
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
    TRIS_MD_TX = 0;
    LATC7 = 0;
    RC1IE = 0;
}

void md_uart_send(unsigned char data)
{
    while (!TRMT1);
    TX1REG = data;
}


// error code reporting: 1 = got OK. 2 = waiting carat.
// 3 = waiting for ok. 4 = error. 5 = timed out.
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


void rf_uart_send(uint8_t data)
{
    while (!TRMT2);
    TX2REG = data;
}


void send_uart_ack()
{
    init_uart();
    // for backwards compatibility, for now
    rf_uart_send('$');
    rf_uart_send('A');
    rf_uart_send('\r');
    rf_uart_send('\n');
}