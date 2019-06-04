
#include "config.h"


void enable_md_uart()
{
    TX_MD = 1;
    TX_MD_TRIS = OUTPUT;        // This is the pic's Tx, the modem's Rx
    RX_MD = 1;
    RX_MD_TRIS = INPUT;         // This is the pic's Rx, the modem's Tx
    
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


void disable_md_uart()
{
    // set to output then disable
    TX_MD_TRIS = OUTPUT;
    RX_MD_TRIS = OUTPUT;
    
    RC1STAbits.SPEN = 0;        // Disable Serial port
    
    RCSTA1bits.CREN = 0;        // Disable receiver of UART module 1
    TXSTA1bits.TXEN = 0;        // Disable Transmit on UART module 1
}


void enable_md_rxint()
{
    GIE = 0;
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    INTCONbits.PEIE = 1;
    GIE = 1;
}
