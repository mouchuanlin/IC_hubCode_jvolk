/* 
 * File:   main.c
 * Author: JV
 *
 * Created on June 12, 2018, 3:16 PM
 */

#include "config.h"
#include "uart.h"
#include "modem.h"


uint8_t currentChar, lastChar;

void __interrupt isr()
{
    if (RC1IF && PIE1bits.RCIE)
    {
        RC1IF = 0;
        currentChar = RC1REG;
        mdRxBuf[endPtr++] = currentChar;
        if (endPtr >= sizeof(mdRxBuf))
            endPtr--;
        if (endPtr > 2)
        {
            // Check for OK or ERROR
            if (mdRxBuf[(uint8_t)(endPtr - 3)] == 'O' && mdRxBuf[(uint8_t)(endPtr - 2)] == 'K' \
                    && (mdRxBuf[(uint8_t)(endPtr - 1)] == '\r' || mdRxBuf[(uint8_t)(endPtr - 1)] == '\n'))
            {
//                tmr3_cnt = 0;
//                disable_tmr3();             // Turn off timer & disable associated interrupt
                receivedOK = true;
                receivedERR = false;
                timedOut = false;
                disable_md_rxint();
            }
            else if (endPtr >= 5 && mdRxBuf[(uint8_t)(endPtr - 5)] == 'E' && \
                     mdRxBuf[(uint8_t)(endPtr - 4)] == 'R' && mdRxBuf[(uint8_t)(endPtr - 3)] == 'R' && \
                     mdRxBuf[(uint8_t)(endPtr - 2)] == 'O' && mdRxBuf[endPtr - 1] == 'R')
            {
//                tmr3_cnt = 0;
//                disable_tmr3();             // Don't turn off timer or disable associated interrupt yet
                receivedOK = false;
                receivedERR = true;
                timedOut = false;
                disable_md_rxint();
            }
        }
        else if (currentChar == '>')
        {
            tmr3_cnt = 0;
            disable_tmr3();
            disable_md_rxint();
            receivedOK = false;
            receivedERR = false;
            receivedCarat = true;
        }
    }
    // UART time-out and modem boot timer
    if (TMR3IF)
    {
        // Timer 3 interrupts every 1s
        tmr3_cnt++;
        TMR3IF = 0;
//        reload_tmr3_1s();
        check_mdstate_tmr3();
    }
}

void main()
{
    init_pic();
//    md_state = BOOT;      // Add 'BOOT' stage for initial 'listening mode', if desired
//    init_int();
//    start_modem_send_process();
    
    while(1)
    {
        if (TEST_BUTTON)
        {
            __delay_ms(50);
            if (TEST_BUTTON)
            {
                start_modem_send_process('T');
            }
        }
        check_modem_state();
    }
}

void init_pic()
{
        /* OSC */
    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b101;    // 110 is 8MHz; 0b101 is 4MHz
    OSCCONbits.OSTS = 1;        // Clk defined by Fosc
//    OSCCON = 0x68;
    /* PORTS */
//PORTA
    ANSELA = 0x01;              // batt voltage/2 through V-divider on RA0
    TRISA = 0b00000001;
    //    BAT_DECT = 1;
    BAT_DECT_TRIS = INPUT;
    MD_POWER_TRIS = OUTPUT;
    MD_POWER = 0;

//PORTC
    ANSELC = 0x00;    
    TRISC = 0b10000000;
    G_LED = 1;      
    G_LED_TRIS = OUTPUT;
    TX_MD = 1;
    TX_MD_TRIS = OUTPUT;        // This is the modem's Rx, the pic's Tx
    RX_MD = 1;
    RX_MD_TRIS = INPUT;         // This is the modem's Tx, the pic's Rx
    
//PORTB
    ANSELB = 0x00;    
    TRISB = 0b10110111;
//    STANDBY_PIN = 1;              // Not used
    STANDBY_PIN_TRIS = OUTPUT;
    SELECT_TRIS = INPUT;
    ALARM_PIN = 1;
    ALARM_PIN_TRIS = INPUT;
    ERROR_PIN = 1;
    ERROR_PIN_TRIS = INPUT;
    B_LED = 1;
    B_LED_TRIS = OUTPUT;
    TEST_BUTTON = 1;
    TEST_BUTTON_TRIS = INPUT;
    WIFI_INT = 1;
    WIFI_INT_TRIS = INPUT;    
    TX_WIFI = 1;
    TX_WIFI_TRIS = OUTPUT;
    RX_WIFI = 1;
    RX_WIFI_TRIS = INPUT;
    
    MD_RESET_TRIS = OUTPUT;
    MD_RESET = 0;
    
    /* INTERRUPTS */
    /* t0con = 0x87: enable timer0, use as 16-bit counter, transition on
     * internal instruction cycle, assign prescaler of: 1:256.
     * Timer0 used to control LEDs.
     */
    T0CON = TMR0_CFG;             //1*4000 = 50,000us
    TMR0H = ((65535-TMR0_HI_FREQ)/256);
    TMR0L = ((65535-TMR0_HI_FREQ)%256);
    TMR0IF = 0;
//    TMR0IE = 1;
//    
//    INTEDG1 = 1;            // Interrupt on rising edge of Alarm
//    INTEDG2 = 1;            // Interrupt on rising edge of "Error"/tamper
//    INT1IP = 1;             // Assign high priority to alarm
//    INT2IP = 1;             // Assign high priority to tamper
//    IOCBbits.IOCB4 = 1;     // Interrupt on Test signal
//    
//    INT1IE = 1;             // Enable Alarm interrupt
//    INT2IE = 1;             // Enable Tamper interrupt
//    INTCONbits.RBIE = 1;    // Enable Test interrupt
//    
//    ALARM_INT = 0;
//    TAMPER_INT = 0;
//    TEST_INT = 0;
//    
//    INTCONbits.PEIE = 1;    // Enable Low and High priority interrupts
//    INTCONbits.GIE = 1;     // Enable HIGH-priority interrupts only
    
//    ledPattern = STANDBY;
    
    
    T3CON = 0b00000101;     // 1:1 prescale on Fosc/4, clk src is internal instruction cycle,
                            // get value by two 8-bit reads, timer3 ON
    T3CONbits.TMR3ON = 0;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    
    md_state = IDLE;
}

void reload_tmr3_10ms()
{
    T3CONbits.TMR3ON = 1;
//    INTCONbits.GIE = 0;
    TMR3H = 0xD8;
    TMR3L = 0xEF;
//    INTCONbits.GIE = 1;
}


void reload_tmr3_1s()
{
    T3CONbits.TMR3ON = 1;
}

void disable_tmr3()
{
    tmr3_cnt = 0;
    T3CONbits.TMR3ON = 0;
//    PIE2bits.TMR3IE = 0;
//    PIR2bits.TMR3IF = 0;
}

void check_mdstate_tmr3()
{
    switch (md_state)
    {
        /* DEBUG ONLY */
        case IDLE:
            G_LED = (bool)(tmr3_cnt % 2);
            if (tmr3_cnt >= _T3_1s)
            {
                disable_tmr3();
                tmr3_cnt = 0;
                receivedOK = false;
                timedOut = true;
            }
            break;
            /* END DEBUG */
            
        case WAITING_FOR_BOOT:
            B_LED = (bool)(tmr3_cnt % 2);
            if (tmr3_cnt >= _T3_45s)       // 4ticks per sec, 28-30 secs to boot
            {
                disable_tmr3();             // Turn off timer & disable associated interrupt
                tmr3_cnt = 0;
                bootTimerIsDone = true;
            }
            break;
        case CONFIG:
            if (tmr3_cnt >= _T3_1s)
            {
                disable_tmr3();             // Turn off timer & disable associated interrupt
                tmr3_cnt = 0;
                receivedOK = false;
//                receivedERR = false;
                timedOut = true;
            }
            break;
        case WAITING_PDP_ACTIVE:
            if (tmr3_cnt >= _T3_10s)
            {
                disable_tmr3();
                tmr3_cnt = 0;
                receivedOK = false;
//                receivedERR = false;
                timedOut = true;
            }
            break;
        case SENDING_DATA:
            if (tmr3_cnt >= _T3_10s)
            {
                disable_tmr3();
                tmr3_cnt = 0;
                receivedOK = false;
//                receivedERR = false;
                timedOut = true;
            }
            break;
    }
}