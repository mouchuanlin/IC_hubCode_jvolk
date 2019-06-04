/*
 * File: modem.c
 * Author: JV
 * 
 * Date Created: 04 May 2018, 11:36 AM
 * Last Modified: 04 May 2018, 11:36 AM
 */

#include "modem.h"



void start_modem()
{
    MD_POWER = PWR_ON;
    // Delay by 30s for module startup
    for (uint8_t i = 0; i < 30; i ++)
        delay5ms(200);
    while (!startup_modem_ok())
        restart_modem();
    
}


bool startup_modem_ok()
{
    init_uart_md();
    if (!wakeup_modem_ok())
        return false;
    if (!setup_modem_io_ok())
        return false;
    return true;
}


void restart_modem()
{
    MD_POWER = PWR_OFF;
    __delay_ms(5000);
    MD_POWER = PWR_OFF;
    __delay_ms(15000);
    __delay_ms(15000);
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



bool wakeup_modem_ok()
{
    const uint8_t at[] = "AT\r\n";
    const uint8_t ipr[] = "AT+IPR=115200\r\n";
    bool cmds_ok;
    uint8_t cnt = 20;
    while (!md_uart_send_ok((uint8_t *)&at[0],sizeof(at), returnStr,YES,40) && --cnt > 0)
        NOP();
    if (cnt == 0)
        return false;
    
    cnt = 20;
    while (!md_uart_send_ok((uint8_t *)&ipr[0],sizeof(ipr), returnStr,YES,40) && --cnt > 0);
    if (cnt == 0)
        return false;
    else
        return true;
}



bool setup_modem_io_ok()
{
    if (is_first_run())
    {
        write_defaults();
        if (!init_modem_io_ok())
            return false;
        save_first_run();
    }
    if (sim_card_ok())
        return true;
    else
        return false;
}


bool init_modem_io_ok()
{
    uint8_t cnt = 20;
#ifdef LE910NA1
    const uint8_t ledCfg[] = "AT#GPIO=1,0,2\r\n";
#endif
#ifdef LE866SV1
    const uint8_t ledCfg[] = "AT#GPIO=7,0,2\r\n";
#endif
    const uint8_t queryVersion[] = "AT#CGMR\r\n";
    const uint8_t ledPattern[] = "AT#SLED=4\r\n";
    
    while (!md_uart_send_ok((uint8_t *)&ledCfg[0],sizeof(ledCfg),returnStr,YES,40) && --cnt > 0);
    if (cnt == 0)
        return false;
    cnt = 20;
    while (!md_uart_send_ok((uint8_t *)&queryVersion[0],sizeof(queryVersion),returnStr,YES,40) && --cnt > 0);
    if (cnt == 0)
        return false;
    cnt = 20;
    while (!md_uart_send_ok((uint8_t *)&ledPattern[0],sizeof(ledPattern),returnStr,YES,40) && --cnt > 0);
    if (cnt == 0)
        return false;
    
    return true;
}


bool sim_card_ok()
{
    const uint8_t cpin[] = "AT+CPIN?\r\n";            // Command string
    const uint8_t cpinResponse[] = "+CPIN: READY";    // Expected return string, if SIM card is ok
    bool matches = false;
    uint8_t buf[32], temp, i, cnt = 20;
    
    while (!md_uart_send_ok((uint8_t *)&cpin[0], sizeof(cpin), returnStr, YES, 40) && --cnt > 0);
    if (cnt == 0)
        return false;
    if (str_matches((uint8_t *)&returnStr[0], (uint8_t *)&cpinResponse[0], sizeof(cpinResponse)))
        return true;
    else
        return false;
    
}


