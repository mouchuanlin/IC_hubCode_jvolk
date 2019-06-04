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
            if (mdRxBuf[(uint8_t)(endPtr - 2)] == 'O' && mdRxBuf[(uint8_t)(endPtr - 1)] == 'K' \
                    && (mdRxBuf[endPtr] == '\r' || mdRxBuf[endPtr] == '\n'))
            {
                tmr5_cnt = 0;
                disable_tmr5();             // Turn off timer & disable associated interrupt
                TMR5IF = 0;
                receivedOK = true;
                receivedERR = false;
                timedOut = false;
                disable_md_rxint();
            }
            else if (endPtr >= 5 && mdRxBuf[(uint8_t)(endPtr - 4)] == 'E' && \
                     mdRxBuf[(uint8_t)(endPtr - 3)] == 'R' && mdRxBuf[(uint8_t)(endPtr - 2)] == 'R' && \
                     mdRxBuf[(uint8_t)(endPtr - 1)] == 'O' && mdRxBuf[endPtr] == 'R')
            {
                tmr5_cnt = 0;
                disable_tmr5();             // Turn off timer & disable associated interrupt
                TMR5IF = 0;
                receivedOK = false;
                receivedERR = true;
                timedOut = false;
                disable_md_rxint();
            }
        }
    }
    // UART time-out and modem boot timer
    if (TMR5IF)
    {
        // timer 5 interrupts every 250ms
        tmr5_cnt++;
        TMR5IF = 0;
        reload_tmr5_pt25s();
        // 2s timer; 
        check_mdstate_tmr5();
    }
}

void main()
{
    init_pic();
//    init_int();
//    start_modem_send_process();
    
    while(1)
    {
        if (TEST_BUTTON)
        {
            __delay_ms(50);
            if (TEST_BUTTON)
            {
                start_modem_send_process();
            }
        }
        check_modem_state();
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
//    BAT_DECT = 1;
    BAT_DECT_TRIS = INPUT;
    MD_POWER_TRIS = OUTPUT;
    MD_POWER = 0;

//PORTC
    ANSELC = 0x00;    
    G_LED = 1;      
    G_LED_TRIS = OUTPUT;
    TX_MD = 1;
    TX_MD_TRIS = OUTPUT;        // This is the modem's Rx, the pic's Tx
    RX_MD = 1;
    RX_MD_TRIS = INPUT;         // This is the modem's Tx, the pic's Rx
    
//PORTB
    ANSELB = 0x00;    
//    STANDBY_PIN = 1;              // Not used
    STANDBY_PIN_TRIS = INPUT;
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
    
    MD_RESET = 0;
    MD_RESET_TRIS = OUTPUT;
    
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
    md_state = IDLE;
}




void reload_tmr5_pt25s()
{
    INTCONbits.GIE = 0;
    T5CON = 0b00110101;     // 1:8 prescale, clk src is internal instruction cycle,
                            // get value by two 8-bit reads, timer5 ON
    PIE5bits.TMR5IE = 1;
    INTCONbits.PEIE = 1;
    PIR5bits.TMR5IF = 0;
    TMR5H = 0x0B;
    TMR5L = 0x45;
    INTCONbits.GIE = 1;
}

void disable_tmr5()
{
    tmr5_cnt = 0;
    T5CONbits.TMR5ON = 0;
    PIE5bits.TMR5IE = 0;
}


void check_mdstate_tmr5()
{
    switch (md_state)
    {
        case WAITING_FOR_BOOT:
            B_LED = (bool)(tmr5_cnt % 2);
            if (tmr5_cnt >= _T5_45s)       // 4ticks per sec, 28-30 secs to boot
            {
                disable_tmr5();             // Turn off timer & disable associated interrupt
                tmr5_cnt = 0;
                bootTimerIsDone = true;
            }
            break;
        case CONFIG:
            if (tmr5_cnt >= _T5_2s)
            {
                disable_tmr5();             // Turn off timer & disable associated interrupt
                tmr5_cnt = 0;
                receivedOK = false;
                receivedERR = false;
                timedOut = true;
            }
            break;
        case SET_UP_DATA:
            if (tmr5_cnt >= _T5_10s)
            {
                disable_tmr5();
                tmr5_cnt = 0;
                receivedOK = false;
                receivedERR = false;
                timedOut = true;
            }
            break;
        case WAITING_PDP_ACTIVE:
            if (tmr5_cnt >= _T5_10s)
            {
                disable_tmr5();
                tmr5_cnt = 0;
                receivedOK = false;
                receivedERR = false;
                timedOut = true;
            }
            break;
        case SENDING_DATA:
            if (tmr5_cnt >= _T5_10s)
            {
                disable_tmr5();
                tmr5_cnt = 0;
                receivedOK = false;
                receivedERR = false;
                timedOut = true;
            }
            break;
        case WAITING_ACK:
            if (tmr5_cnt >= _T5_10s)
            {
                disable_tmr5();
                tmr5_cnt = 0;
                receivedOK = false;
                receivedERR = false;
                timedOut = true;
            }
            break;
    }
}