/*
 * File: modem.c
 * Author: THE MASTER
 * 
 * This file contains all functions universal to modem operation such as modem
 * initialization and configuration.
 * 
 * Date Created: 02 May 2018, 09:12 AM
 * Last Modified: 02 May 2018, 09:12 AM
 */

#include "config.h"
#include "default_settings.h"


void start_modem()
{
    CLRWDT();
    MD_POWER = PWR_ON;
    __delay_ms(25000);
    while (!md_config_ok())
        restart_modem();
    md_started = true;
}

void restart_modem()
{
    CLRWDT();
    MD_POWER = PWR_OFF;
    __delay_ms(5000);
    MD_POWER = PWR_ON;
    __delay_ms(25000);
}

void stop_modem()
{
    MD_POWER = PWR_OFF;
    __delay_ms(5000);
    md_started = false;
}


bool md_config_ok()
{
    
}