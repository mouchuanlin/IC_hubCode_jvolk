/* 
 * File:   main.c
 * Author: Jennifer
 *
 * This program intends to cleanly wake the module and send out a single text
 * message over the cellular network.
 * 
 * Created on April 24, 2018, 11:00 AM
 */

#include "config.h"
#include "queue.h"

void __interrupt isr()
{
//    if (ALARM_INT && !stillTriggered){}
//    if (TAMPER_INT && !stillTampered){}
    if (TEST_INT)       // Count button presses on rising edge; increment buttonPressCount. 
    {                   // Time out after 2s without edge detection; reset button 
        INTCONbits.RBIF = 0;
        PIE2bits.TMR3IE = 1;
        T3CONbits.TMR3ON = 1;
        reload_timer3_2s();
        buttonPressCount++;                // press count if it times out.
        if (!inButtonMenu && buttonPressCount == 5)
        {
            inButtonMenu = true;
            reload_timer3_5s();
            buttonPressCount = 0;
        }
    }
    if (TMR0IF)
    {
        TMR0IF = 0;
        control_md_state();
        reload_timer0();
    }
    if (TMR3IF && ((!inButtonMenu && buttonPressCount > 5) || (inButtonMenu && buttonPressCount >= 4)))            // press 4x in menu to exit
    {
        inButtonMenu = false;
        TMR3IF = 0;
        buttonPressCount = 0;
        T3CONbits.TMR3ON = 0;               // Turn off timer3
        PIE2bits.TMR3IE = 0;
        MODEM_STATE = IDLE_Q_EMPTY;
    }
    else if (TMR3IF && inButtonMenu && buttonPressCount == 3)
    {
        reload_timer3_5s();
        enableTransmitLEDs = true;           // test
        buttonPressCount = 0;
    }
}

/*
 * 
 */
void main() 
{
    init_pic();
//    init_rx_uart();
    init_queue();
    start_timer0();
    reload_timer0();
    enable_timer3();
    reload_timer3_2s();
    init_int();             // Enable interrupts
    
    WDTCONbits.SWDTEN = 0;
    CLRWDT();
    
    /* TEMP */
//    wake_modem();
    /* END TEMP */
    
    while (1)
    {
        
//    while (!TMR0IF)
//    {
//        TMR0IF = 0;
//        control_md_state();
//    }
//        if (!queue.isEmpty())
//            run_modem();
        
//        else
//        {
//            while (!RCONbits.nTO && ready_for_sleep)
//            {
//                DISABLE_MODEM();      // (MD_POWER = OFF;)
//                SLEEP();							// if woken from sleep, is caused by alarm/tamper/test/etc external interrupt
//													 such that ISR is checked, items added to Queue for modem to transmit
//				NOP();
//            }
//        }
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
    TX_MD_TRIS = OUTPUT;        // This is the pic's Tx, the modem's Rx
    RX_MD = 1;
    RX_MD_TRIS = INPUT;         // This is the pic's Rx, the modem's Tx
    
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
    
    
    MODEM_STATE = IDLE_Q_EMPTY;
}
    
void init_int()
{
    /* INTERRUPTS */
//    
//    INTEDG1 = 1;            // Interrupt on rising edge of Alarm
//    INTEDG2 = 1;            // Interrupt on rising edge of "Error"/tamper
//    INT1IP = 1;             // Assign high priority to alarm
//    INT2IP = 1;             // Assign high priority to tamper
    IOCBbits.IOCB4 = 1;     // Interrupt on Test signal
//    
//    INT1IE = 1;             // Enable Alarm interrupt
//    INT2IE = 1;             // Enable Tamper interrupt
    INTCONbits.RBIE = 1;    // Enable Test interrupt
//    
//    ALARM_INT = 0;
//    TAMPER_INT = 0;
    TEST_INT = 0;
//    
    INTCONbits.PEIE = 1;    // Enable Low and High priority interrupts
    INTCONbits.GIE = 1;     // Enable HIGH-priority interrupts only
}

void start_timer0()
{
    /* t0con = 0x87: enable timer0, use as 16-bit counter, transition on
     * internal instruction cycle, assign prescaler of: 1:256.
     * Timer0 used to control LEDs.
     */
    T0CON = TMR0_CFG;             //1*4000 = 50,000us
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


void enable_timer3()
{
    T3CON = 0b00110001;//    0x71;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    INTCONbits.GIE = 1;
}
void reload_timer3_2s()
{
    TMR3H = 0x30;
    TMR3L = 0;
}
void reload_timer3_5s()
{
    TMR3H = 0x78;
    TMR3L = 0;
}