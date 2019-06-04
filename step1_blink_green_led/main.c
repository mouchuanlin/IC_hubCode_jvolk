/* 
 * File:   main.c
 * Author: Scott
 *
 * Created on October 16, 2018, 5:00 PM
 */


#include "config.h"
#include "modem.h"
#include "state.h"

/*
 * 
 */
void __interrupt isr()
{
    if (TMR0IF)
    {
        TMR0IF = 0;
        reload_timer0();
        gled_tmr0_tick++;
        bled_tmr0_tick++;
        control_leds();
    }
    if (PIR2bits.TMR3IF)
    {
        PIR2bits.TMR3IF = 0;
        if (waitingForModemBoot)
        {
            if (tmr3_cnt++ >= T3_1S)
            {
                tmr3_cnt = 0;
                _1sTmr3Cnt++;
            }
            if (_1sTmr3Cnt >= T3_25S)
            {
                _25sTmr3Cnt++;
                if (_25sTmr3Cnt == 1)
                {
                    G_LED = 0;
                    modemMayBeAwake = true;
                }
                if (_25sTmr3Cnt >= 2)
                {
                    B_LED = 0;
                    modemMayBeAwake = false;
                    _25sTmr3Cnt = 0;
                }
            }
        }
    }
}



void main() 
{
    init_pic();
//    init_md_uart();
    start_modem();
    while(1)
    {
        check_state();
//        SLEEP();
//        NOP();
    }
}



void init_pic()
{
    /* OSC */
    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b110;    // 8MHz; 0b101 is 4MHz
    OSCCONbits.OSTS = 1;        // Clk defined by Fosc
//    OSCCON = 0x68;
    /* PORTS */
//PORTA
    ANSELA = 0x01;              // batt voltage/2 through V-divider on RA0
    BAT_DECT_TRIS = INPUT;
    MD_PWR_TRIS = OUTPUT;
    MD_PWR = 0;

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
    ALARM_PIN = 1;
    ALARM_PIN_TRIS = INPUT;
    ERROR_PIN = 1;
    ERROR_PIN_TRIS = INPUT;
    B_LED = 1;
    B_LED_TRIS = OUTPUT;
    TEST_PIN = 1;
    TEST_PIN_TRIS = INPUT;
    WIFI_INT = 1;
    WIFI_INT_TRIS = INPUT;    
    TX_WIFI = 1;
    TX_WIFI_TRIS = OUTPUT;
    RX_WIFI = 1;
    RX_WIFI_TRIS = INPUT;
    
    MD_RESET = 0;
    MD_RESET_TRIS = OUTPUT;
    
    enable_ints();
    STATE = IDLE;
}

void enable_ints()
{
    INTCONbits.GIE = 0;
    
    /* Timer 0, used for LEDs */
    start_timer0();                 // timer0 is inactive during sleep
    
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;
    
    INTCONbits.GIE = 1;
}

void start_timer0()
{
    T0CON = 0x87;             //1*4000 = 50,000us
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    INTCONbits.GIE = 1;
}


void reload_timer0()
{
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
}

void reload_tmr3_pt25s()
{
    T3CONbits.TMR3ON = 1;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    TMR3H = 0x0B;
    TMR3L = 0x45;
}

void disable_tmr3()
{
//    buttonPressCount = 0;
    tmr3_cnt = 0;
    T3CONbits.TMR3ON = 0;
    PIE2bits.TMR3IE = 0;
}
