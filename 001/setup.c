/* 
 * File:   setup.c
 * Author: Jennifer
 * 
 * This file contains all functions related to default module configuration.
 *
 * Created on April 9, 2018, 3:12 PM
 */

#include "config.h"
#include "default_setup.h"
#include "module.h"
#include "uart.h"
#include "eeprom.h"

uint8_t saveINTCONbits;

void write_defaults()
{
    saveINTCONbits = INTCON;
    INTCONbits.GIE = 0;
    INTCONbits.GIEH = 0;
    INTCONbits.GIEL = 0;
    
    uint8_t page, addr, i, temp;
    
//---------Check Version-----------
    if( ee_read(0, 0x00)==VERSION[0] )
    {
        if( ee_read(0, 0x01)==VERSION[1] )
        {
            if( ee_read(0, 0x02)==VERSION[2] )
                return;
        }
    }
    
    /************ APN ************/
    page = 0x00;
    addr = APN_ADDR;
    i = 0;
    while (i++ < 0x20 && temp != '#')
    {
        temp = APN[i];
        ee_write(page, (uint8_t)(addr + i), temp);
    }
    /************ IP1 ************/
    page = 0x00;
    addr = IP1_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = IP1[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i++;
    }
    /************ IP2 ************/
    page = 0x00;
    addr = IP2_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = IP2[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i++;
    }
    /************ IP3 ************/
    page = 0x00;
    addr = IP3_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = IP3[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i++;
    }
    /************ IP4 ************/
    page = 0x00;
    addr = IP4_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = IP4[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i++;
    }
    
    /*********** PORT1 ***********/
    page = 0x00;
    addr = PORT1_ADDR;
    ee_write(page, addr, (PORT1 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (PORT1 & 0x00FF));
    /*********** PORT2 ***********/
    page = 0x00;
    addr = PORT2_ADDR;
    ee_write(page, addr, (PORT2 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (PORT2 & 0x00FF));
    /*********** PORT3 ***********/
    page = 0x00;
    addr = PORT3_ADDR;
    ee_write(page, addr, (PORT3 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (PORT3 & 0x00FF));
    /*********** PORT4 ***********/
    page = 0x00;
    addr = PORT4_ADDR;
    ee_write(page, addr, (PORT4 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (PORT4 & 0x00FF));
    
    /******** ACCESS CODE ********/
    page = 0x00;
    addr = ACCESS_CODE_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = ACCESS_CODE[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i ++;
    }
    
    /******** PROGRAM ACK ********/
    ee_write(0x00, PROGRAM_ACK_ADDR, PROGRAM_ACK);
    /******* TESTING FREQ ********/
    ee_write(0x00, TESTING_FREQ_ADDR, TEST_FREQ);
    /***** SERVER ACK TIME *******/
    ee_write(0x00, SERVER_ACK_TIME_ADDR, SERVER_ACK_TIME);
    /******* SMS WAIT TIME *******/
    ee_write(0x00, SMS_WAIT_TIME_ADDR, SMS_WAIT_TIME);
    /******** UNIT ACCOUNT *******/
    page = 0x00;
    addr = UNIT_ACCT_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = UNIT_ACCT[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i ++;
    }
    /********* LINE CARD *********/
    page = 0x00;
    addr = LINE_CARD_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = LINE_CARD[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i ++;
    }
    /*********** ZONE ************/
    ee_write(0, ZONE1_ADDR, ZONE1);         // 12#
    ee_write(0, ZONE2_ADDR, ZONE2);         // 13#
    ee_write(0, TP_PIN_ADDR, TP_PIN);       // 14#
    ee_write(0, CYCLE_ADDR, CYCLE);         // 15#
    ee_write(0, RETRY_TIMES_ADDR, RETRY);   // 16#
    /******** ENCRYPTION *********/
    ee_write(0, ENCRYPTION_ADDR,ENCRYPTION);         // 95#
    
    ee_write(0, 0x0F, 0x00);
    
    ee_write(0, 0x00, VERSION[0]);
    ee_write(0, 0x01, VERSION[1]);
    ee_write(0, 0x02, VERSION[2]);
    
    ee_write(page, FIRST_RUN_CHECK, 0x57);
}


void setup_timer3()
{
    T3CON = 0x71;
    TMR3H = 0x40;   //50ms
    TMR3L = 0;
}