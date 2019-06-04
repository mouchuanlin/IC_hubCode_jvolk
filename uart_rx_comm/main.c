/* 
 * File:   main.c
 * Author: THE MASTER
 *
 * Created on May 17, 2018, 4:44 PM
 */

#include "config.h"

/*
 * 
 */
uint8_t rxBuf[10];
uint8_t pos = 0, temp;
bool stayAwake = false, processData = false;

void __interrupt isr()
{
    if (INTCONbits.RBIF)
    {
        INTCONbits.RBIF = 0;
        stayAwake = true;
//        enable_RX_uart_interrupt();
//        start_uart();
    }
    if (RC2IF)
    {
        RC2IF = 0;
        rxBuf[pos++] = RC2REG;
        if (pos == 7)
        {
            processData = true;
            pos = 0;
        }
    }
}


void main() 
{
    init_pic();
    init_int();
    start_uart();
    enable_RX_uart_interrupt();
    while (1)
    {
//        SLEEP();
//        NOP();
//        else
//            G_OFF();
        if (processData)
        {
            G_ON();
            __delay_ms(200);
            G_OFF();
//            out_sbuf2('$');
//            out_sbuf2(0x41);
//            out_sbuf2(0x0d);
//            out_sbuf2(0x0a);
        }
    }
}

void init_pic()
{
    /* OSC */
    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b110;        // 8MHz
    OSCCONbits.OSTS = 1;            // Clk defined by Fosc
    
    /* PORTS */
    ANSELA = 0x01;
    BAT_DECT_TRIS = INPUT;
    MD_POWER_TRIS = OUTPUT;
    MD_POWER = 0;

//PORTC
    ANSELC = 0x00;    
    G_LED = 1;      
    G_LED_TRIS = OUTPUT;
    TX_MD = 1;
    TX_MD_TRIS = OUTPUT;        // This is the pic's Tx, the modem's Rx
    RX_MD = 1;
    RX_MD_TRIS = INPUT;         // This is the pic's Rx, the modem's Tx
    
//PORTB
    ANSELB = 0x00;    
    STANDBY_PIN_TRIS = INPUT;
    SELECT_TRIS = INPUT;
//    ALARM_PIN = 1;
    ALARM_PIN_TRIS = INPUT;
//    ERROR_PIN = 1;
    ERROR_PIN_TRIS = INPUT;
    B_LED = 1;
    B_LED_TRIS = OUTPUT;
//    TEST_PIN = 1;
    TEST_PIN_TRIS = INPUT;
//    WIFI_INT = 1;
    WIFI_INT_TRIS = INPUT;    
    TX_WIFI = 1;
    TX_WIFI_TRIS = OUTPUT;
//    RX_WIFI = 1;
    RX_WIFI_TRIS = INPUT;
    
    MD_RESET = 0;
    MD_RESET_TRIS = OUTPUT;
    
}


void init_int()
{
//    IOCBbits.IOCB5 = 1;     // Interrupt on Rx UART signal
    IOCBbits.IOCB4 = 1;     // Interrupt on Test button signal
    INTCONbits.RBIE = 1;    // Enable Rx interrupt
    INTCONbits.RBIF = 0;
    INTCONbits.PEIE = 1;
    IPEN = 0;
//    INTCON2bits.RBIP = 1;       // high priority
//    INTCONbits.GIE = 1;
    
    IOCB = 0x10;    //RB.4 IOC enable
    
    RBIE = 1;       //Enable IOC interrupt
    PEIE = 1;       //Enable LOW&HIGH Priority interrupt
    GIE = 1;        //Enable HIGH Priority interrupt
}



//if( RC2IE==1&&RC2IF==1 )
//    {
//        do{       	
//            temp = RC2REG;
//            LED_G = 1;
//            rx2_buf[rx2_cnt] = temp;
//            if( ++rx2_cnt>=10 )
//                rx2_cnt = 9;
//            if( temp=='\n' )
//            {
//                if( rx2_buf[0]=='$'&&rx2_buf[1]=='R'&&rx2_buf[4]=='\r'&&rx2_buf[5]=='\n' )     //rf data in
//                {
//                    if( rx2_buf[3]<'0'||rx2_buf[3]>'9' )
//                        rx2_buf[3] = '0';
//                    if( rx2_buf[2]==0x46)//'F'
//                        add_even(FLOOD_T,rx2_buf[3]); 
//                    else if( rx2_buf[2]==0x53)//'S'
//                        add_even(SMOKE_ALARM_T,rx2_buf[3]);
//                    else if( rx2_buf[2]==0x47)//'G'
//                        add_even(GLASS_T,rx2_buf[3]);
//                        
//                    out_sbuf2('$');
//                    out_sbuf2(0x41);
//                    out_sbuf2(0x0d);
//                    out_sbuf2(0x0a);
//                }
//                rx2_cnt = 0;
//            }
//        }while(RC2IF==1);
//    }