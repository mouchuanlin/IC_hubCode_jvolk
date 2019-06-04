/* 
 * File:   main.c
 * Author: Jennifer
 * 
 * This program intends to replace the code originally written for the smoke 
 * hub from Gainwise. The LED patterns and button-press combinations are the
 * same but the layout is different at large.
 *
 * Created on April 4, 2018, 5:33 PM
 */

#include "config.h"
#include "eeprom.h"
#include "uart.h"
#include "queue.h"
#include "module.h"
#include "default_setup.h"
#include "sms.h"

/*
 * 
 */
uint8_t JENS_NUMBER[] = "8056405127";

void main()
{
    init_pic();
//    init_uart();
        while (!startup_md_ok())
            md_restart();
//    init_interrupts();            // Initialize interrupts/enable tamper and 
                                    // alarm only after module has been started 
                                    // for the first time. This ensures that no
                                    // interrupts occur on startup (which can
                                    // disturb the "natural" order of operations)
        send_text((uint8_t *)"Testing smoke hub - Jen's code, Kore SIM.\r\n", 42, (uint8_t *)&JENS_NUMBER[0]);
    while(1)
    {
    
        G_LED = 0;
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
    
    /* INTERRUPTS */
    /* t0con = 0x87: enable timer0, use as 16-bit counter, transition on
     * internal instruction cycle, assign prescaler of: 1:256.
     * Timer0 used to control LEDs.
     */
//    T0CON = TMR0_CFG;             //1*4000 = 50,000us
//    TMR0H = ((65535-TMR0_4MHZ)/256);
//    TMR0L = ((65535-TMR0_4MHZ)%256);
//    TMR0IF = 0;
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
}