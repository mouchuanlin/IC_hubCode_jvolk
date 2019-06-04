
#include "config.h"
#include "uart.h"

void init_uart(void)
{
    TX1STA = 0x26;       //00100110
    RC1STA = 0x90;       //10010000
    BAUDCON1 = 0xc8;     //11001000
    SPBRG1 = 103;       //19200bps
    SPBRGH1 = 0;
    INTCONbits.PEIE = 1;
    PIE1bits.RC1IE = 1;
    INTCONbits.GIE = 1;
    
//    TX2STA = 0x26;       //00100110
//    RC2STA = 0x90;       //10010000
//    BAUDCON2 = 0xc8;     //11001000
//    SPBRG2 = 207;       //9600bps
//    SPBRGH2 = 0;
//    WPUB5 = 1;
//    RC2IE = 1;
}