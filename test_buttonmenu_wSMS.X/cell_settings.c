/*
 * File: default_settings.h
 * Author: JV
 * 
 * Date Created: 04 May 2018, 12:19 PM
 * Last Modified: 04 May 2018, 12:20PM
 */

#include "cell_settings.h"
#include "modem.h"

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
        temp = default_APN[i];
        ee_write(page, (uint8_t)(addr + i), temp);
    }
    /************ IP1 ************/
    page = 0x00;
    addr = IP1_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = default_IP1[i];
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
        temp = default_IP2[i];
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
        temp = default_IP3[i];
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
        temp = default_IP4[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i++;
    }
    
    /*********** PORT1 ***********/
    page = 0x00;
    addr = PORT1_ADDR;
    ee_write(page, addr, (default_PORT1 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (default_PORT1 & 0x00FF));
    /*********** PORT2 ***********/
    page = 0x00;
    addr = PORT2_ADDR;
    ee_write(page, addr, (default_PORT2 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (default_PORT2 & 0x00FF));
    /*********** PORT3 ***********/
    page = 0x00;
    addr = PORT3_ADDR;
    ee_write(page, addr, (default_PORT3 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (default_PORT3 & 0x00FF));
    /*********** PORT4 ***********/
    page = 0x00;
    addr = PORT4_ADDR;
    ee_write(page, addr, (default_PORT4 >> 8));
    ee_write(page, (uint8_t)(addr + 1), (default_PORT4 & 0x00FF));
    
    /******** ACCESS CODE ********/
    page = 0x00;
    addr = ACCESS_CODE_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = default_ACCESS_CODE[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i ++;
    }
    
    /******** PROGRAM ACK ********/
    ee_write(0x00, PROG_ACK_ADDR, default_PROGRAM_ACK);
    /******* TESTING FREQ ********/
    ee_write(0x00, TEST_FREQ_ADDR, default_TEST_FREQ);
    /***** SERVER ACK TIME *******/
    ee_write(0x00, SERVER_ACK_TIME_ADDR, default_SERVER_ACK_TIME);
    /******* SMS WAIT TIME *******/
    ee_write(0x00, SMS_WAIT_TIME_ADDR, default_SMS_WAIT_TIME);
    /******** UNIT ACCOUNT *******/
    page = 0x00;
    addr = UNIT_ACCT_ADDR;
    temp = 0x00;
    i = 0;
    while (temp != '#')
    {
        temp = default_ACCT[i];
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
        temp = default_LINE_CARD[i];
        ee_write(page, (uint8_t)(addr + i), temp);
        i ++;
    }
    /*********** ZONE ************/
    ee_write(0, HUB_ZONE_ADDR, default_ZONE1);          // 12#
//    ee_write(0, ZONE2_ADDR, default_ZONE2);             // 13#
    ee_write(0, TP_PIN_EN_ADDR, default_TP_PIN);        // 14#
    ee_write(0, NUM_NET_RETRY_ADDR, default_CYCLE);     // 15#
    ee_write(0, NUM_IP_RETRY_ADDR, default_RETRY);      // 16#
    /******** ENCRYPTION *********/
    ee_write(0, ENCRYPTION_ADDR,default_ENCRYPTION);    // 95#
    
    ee_write(0, 0x0F, 0x00);
    
    ee_write(0, 0x00, VERSION[0]);
    ee_write(0, 0x01, VERSION[1]);
    ee_write(0, 0x02, VERSION[2]);
}


bool is_first_run()
{
    return (bool)(ee_read(0, FIRST_RUN_CHECK) != 0x57);
}

void save_first_run()
{
    ee_write(0, FIRST_RUN_CHECK, 0x57);
}

bool program_field(uint8_t field_number, uint8_t data_buf[], uint8_t size)
{
    uint8_t start_addr;
    uint8_t page = 0x00;
    uint8_t max_size = 40;
    uint8_t i = 0;
    if (field_number == 1)
        start_addr = IP1_ADDR;
    else if (field_number == 2)
        start_addr = IP2_ADDR;
    else if (field_number == 3)
        start_addr = IP3_ADDR;
    else if (field_number == 4)
        start_addr = IP4_ADDR;
    else if (field_number == 5)
        start_addr = ACCESS_CODE_ADDR;
    else if (field_number == 6)
        start_addr = PROG_ACK_ADDR;
    else if (field_number == 7)
        start_addr = TEST_FREQ_ADDR;
    else if (field_number == 8)
        start_addr = SERVER_ACK_TIME_ADDR;
    else if (field_number == 9)
        start_addr = SMS_WAIT_TIME_ADDR;
    else if (field_number == 10)
        start_addr = UNIT_ACCT_ADDR;
    else if (field_number == 11)
        start_addr = LINE_CARD_ADDR;
    else if (field_number == 12)
        start_addr = HUB_ZONE_ADDR;
    else if (field_number == 14)
        start_addr = TP_PIN_EN_ADDR;
    else if (field_number == 15)
        start_addr = NUM_NET_RETRY_ADDR;
    else if (field_number == 16)
        start_addr = NUM_IP_RETRY_ADDR;
    else if (field_number == 31)
        start_addr = PORT1_ADDR;
    else if (field_number == 32)
        start_addr = PORT2_ADDR;
    else if (field_number == 33)
        start_addr = PORT3_ADDR;
    else if (field_number == 34)
        start_addr = PORT4_ADDR;
    else if (field_number == 35)
        start_addr = APN_ADDR;
    else if (field_number >= 41 && field_number <= 56)
    {
        page = 0x01;
        start_addr = (uint8_t)(ID_START_ADDR + 6 * (field_number - 41));
        max_size = 6;
    }
    if (size <= max_size)
    {
        if (field_number >= 10)
        {
            sms_buf[0] = (uint8_t)(field_number / 0x0A + 0x41);
            sms_buf[1] = (uint8_t)(field_number % 0x0A + 0x30);
        }
        else
            sms_buf[0] = (uint8_t)(field_number + 0x30);
        sms_buf[2] = '#';
        for (i = 0; i < size; i++)
        {
            ee_write(page, start_addr, data_buf[i]);
            sms_buf[(uint8_t)(i + 3)] = data_buf[i];
        }
        sms_buf[(uint8_t)(i + 3)] = '#';
        return true;
    }
    return false;
}
