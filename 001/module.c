/* 
 * File:   module.c
 * Author: Jennifer
 * 
 * This file contains all functions related to general module configuration 
 * and operation.
 *
 * Created on April 5, 2018, 5:23 PM
 */

#include "config.h"
#include "uart.h"
#include "module.h"
#include "eeprom.h"
#include "default_setup.h"


//REMOVE
uint8_t HL_freq = 1;

/* POWER-CYCLE MODEM - LAST RESORT */
void md_restart()
{
    MD_POWER = 0;
    delay5ms(1000);
    MD_POWER = 1;
    start_module_ok();
}


bool wakeup_module_ok()
{
    uint8_t read_buf[20];           // temp buffer used to read from uart
    MD_POWER = 1;   //-----
    uint8_t initStr[] = "AT\r\n";
    uint8_t ipr[] = "AT+IPR=115200\r\n";
    uint8_t cnt = 15;
    bool rsp1_ok, rsp2_ok;
    
    // Delay by 50s for module startup
    for (uint8_t i = 0; i < 50; i ++)
        delay5ms(200);
    
    while (cnt-- > 0 && !rsp1_ok)
        rsp1_ok = md_uart_write_str_ok((unsigned char *)&initStr[0], sizeof(initStr), YES);
    
    cnt = 15;
    while (cnt-- > 0 && !rsp2_ok)
        rsp2_ok = md_uart_write_str_ok((unsigned char *)&ipr[0], sizeof(ipr), YES);
    
    return (bool)(rsp1_ok && rsp2_ok);
}


void delay5ms(unsigned int cnt)         // Based on 4MHz Fosc, or 1MHz instruction cycle
{
    uint8_t i, j;
    while(cnt-- != 0)
    {
        j = 100;
        while(j-- != 0)
        {
            i = 32;//50;
            while(--i != 0);
        }
    }
}

bool is_first_run()
{
    return (bool)(ee_read(0, FIRST_RUN_CHECK) != 0x57);
}

void save_first_run()
{
    ee_write(0, FIRST_RUN_CHECK, 0x57);
}

bool init_modem_ok()
{
    bool c1 = false, c2 = false, c3 = false, c4 = false, c5 = false;
    uint8_t repeat = 20, index = 0;
#ifdef M_910
    const uint8_t ledCfg[] = "AT#GPIO=1,0,2\r\n";
#else
    const uint8_t ledCfg[] = "AT#GPIO=7,0,2\r\n";
#endif
    const uint8_t queryVersion[] = "AT#CGMR\r\n";
    const uint8_t ledPattern[] = "AT#SLED=4\r\n";
    
    while (repeat-- > 0 && !c1)
        c1 = md_uart_write_str_ok((unsigned char *)"ATE1\r\n", 6, YES);         // Turn on echo
    repeat = 20;
    delay5ms(1);
    while (repeat-- > 0 && !c2)
        c2 = md_uart_write_str_ok((unsigned char *)"AT+CMEE=2", 6, YES);        // Turn on verbose error msg
    repeat = 20;
    delay5ms(1);
    while (repeat-- > 0 && !c3)
        c3 = md_uart_write_str_ok((unsigned char *)&ledCfg[0], sizeof(ledCfg), YES);
    repeat = 20;
    delay5ms(1);
    while (repeat-- > 0 && !c4)
        c4 = md_uart_write_str_ok((unsigned char *)&ledPattern[0], sizeof(ledPattern), YES);
    repeat = 20;
    delay5ms(1);
    while (repeat-- > 0 && !c5)
        c5 = md_uart_write_str_ok((unsigned char *)&queryVersion[0], sizeof(queryVersion), YES);    // ver 23.00.004 should be correct as of 10 Apr 2018
    delay5ms(1);

    if(c1 && c2 && c3 && c4)
        return true;
    else
        return false;
}



bool md_io_setup_ok()
{
    if(is_first_run())
    {
        write_defaults();
        if (!init_modem_ok())
            return false;
        save_first_run();
    }
    if (!sim_card_ok())
        return false;
    return true;
}


bool sim_card_ok()
{
    const unsigned char cpin[] = "AT+CPIN?\r\n";            // Command string
    const unsigned char cpinResponse[] = "+CPIN: READY";    // Expected return string, if SIM card is ok
    uint8_t buf[32], temp, i;
    bool matches = false;
    
    if (!md_uart_write_str_ok((unsigned char *)&cpin[0], sizeof(cpin), NO))
        return false;
    RCIE = 0;
    CREN1 = 1;
    while (!TMR3IF)
    {
        setup_timer3();
        if (RC1IF)
        {
            temp = RC1REG;
            buf[i++] = temp;
            if (i >= 32)
                i = 31;
            if (temp == '\n')
            {
                matches = str_matches((uint8_t *)&buf[0], (uint8_t *)&cpinResponse[0], sizeof(cpinResponse));
                if (matches)
                {
                    for (uint8_t j = 0; j < 5; j ++)
                        temp = RC1REG;                      // Clear Rx buffer
                    RC1REG = 1;
                    TMR3ON = 0;                             // Turn off timer 3
                    return true;
                }
            }
        }
    }
    return false;               // If nothing received or string didn't match for any of the attempts
}


bool startup_md_ok()
{
    init_uart_modem();
    if (!wakeup_module_ok())
        return false;
    if (!md_io_setup_ok())
        return false;
    if (!registration_ok())
        return false;
    if (!is_connected())
        return false;
    if (!open_connection())
    {
        // Do something...
    }
    
    return true;
}
