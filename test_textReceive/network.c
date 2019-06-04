/* 
 * File:   network.c
 * Author: Jennifer
 * 
 * This file contains all functions related to network configuration.
 *
 * Created on April 10, 2018, 5:18 PM
 */

#include "config.h"
#include "module.h"
#include "uart.h"
#include "eeprom.h"


bool is_connected()
{
//    if (!socket_configured())       // Configure Socket 1 (don't open yet)
//        return false;
    if (!md_attached()){}             // Perform GPRS attach - for now, don't check if 
//        return false;
//    if (!set_pdp_ok())              // Configure PDP context according to module type
//        return false;
//    if (!open_pdp_ok())
//        return false;
    
    return true;
}

bool registration_ok()
{
    const uint8_t creg[] = "AT+CREG?\r\n";
    const uint8_t creg_resp[] = "+CREG: ";
    const uint8_t cereg[] = "AT+CEREG?\r\n";
    const uint8_t cereg_resp[] = "+CEREG: ";
    
    uint8_t buf[32], i = 0, temp;
    bool matches = true;
    
    for (uint8_t j = 0; j < 4; j ++)                // Repeat same cmds for +CREG and +CEREG
    {
        if (j % 2 == 0)
        {
            if (!md_uart_write_str_ok((unsigned char *)&creg[0], sizeof(creg), NO))
                return false;
        }
        else if (j % 2 == 1)
        {
            if (!md_uart_write_str_ok((unsigned char *)&cereg[0], sizeof(cereg), NO))
                return false;
        }
    
        RCIE = 0;
        CREN1 = 1;
        while (!TMR3IF && !matches)
        {
            setup_timer3();
            if (RC1IF)
            {
                temp = RC1REG;
                buf[i++] = temp;
                if (i >= 32)
                    i = 31;
                if (temp == 'K')
                {
                    if (j == 0)
                        matches &= str_matches((uint8_t *)&buf[(uint8_t)(i - 16)], (uint8_t *)&creg_resp[0], sizeof(creg_resp));
                    else if (j == 1)
                        matches &= str_matches((uint8_t *)&buf[(uint8_t)(i - 17)], (uint8_t *)&cereg_resp[0], sizeof(cereg_resp));
            
                    if (j == 0 && buf[(uint8_t)(i - 9)] == '0')
                        if (!md_uart_write_str_ok((unsigned char *)"AT+CREG=1", sizeof(creg), YES)){}
//                            return false;
                    matches &= (bool)((buf[(uint8_t)(i - 7)] == '1') || (buf[(uint8_t)(i - 7)] == '5'));        // Check if device is registered as GSM compact. 
                                                                            // Not sure if gainwise meant to check the first parameter or the 2nd,
                                                                            // the former of which would reveal registration on the network.
                    i = 0;
                }
                
            }
        }
        delay5ms(200);
    }
    return matches;
}


bool socket_configured()
{
    const uint8_t scfgext[] = "AT#SCFGEXT=1,1,0,0,0,0\r\n";
    if (md_uart_write_str_ok((uint8_t *)&scfg[0], sizeof(scfg), YES))
        return true;
    
    if (md_uart_write_str_ok((uint8_t *)&scfgext[0], sizeof(scfgext), YES))
        return true;
    
    return false;
}

bool md_attached()
{
    const uint8_t gprs_check[] = "AT+CGATT?\r\n";
    const uint8_t gprs_resp[] = "+CGATT: ";
    const uint8_t gprs_attach[] = "AT+CGATT=1";
    
    uint8_t temp, i, buf[40], cnt = 15;
    bool attached = false;              // False before proven true!
    
    while (cnt-- > 0)
    {
        if (md_uart_write_str_ok((uint8_t *)&gprs_check[0], sizeof(gprs_check), YES))
        {
            while (buf[i++] != ':');        // Parse string for ':' delimiter; i will be at the next position (' ')
            if (str_matches((uint8_t *)&buf[(uint8_t)(i - 7)], (uint8_t *)&gprs_resp[0], 7))       // Match accordingly
            {
                attached = (bool)(buf[(uint8_t)(i + 1)] == '1');
                if (!attached)
                {
                    if (!md_uart_write_str_ok((uint8_t *)(&gprs_attach[0]), sizeof(gprs_attach), YES))
                        return false;
                }
                else
                    return true;
            }
            i = 0;
        }
        delay5ms(100);
    }
    return attached;
}

bool set_pdp_ok()
{
    uint8_t apn_buf[], i = 0;
    md_uart_write_str_ok((uint8_t *)(&cgdcont[0]), sizeof(cgdcont), NO);
    while (apn_buf[i] != '#')
    {
        apn_buf[i] = ee_read(0, (uint8_t)(APN_ADDR + i));      // Read entire APN from EEPROM
        i++;
    }
    apn_buf[i++] = '"';         // End the string with proper termination characters
    apn_buf[i++] = '\r';
    apn_buf[i++] = '\n';
    return md_uart_write_str_ok((uint8_t *)(&apn_buf[0]), sizeof(apn_buf), YES);
}

bool open_pdp_ok()
{
    delay5ms(20);
    return md_uart_write_str_ok((uint8_t *)(&sgact[0]), sizeof(sgact), YES);
}



bool open_connection()
{
    const uint8_t sd0[] = "AT#SD=1,0,";
    const uint8_t sd1[] = "\",0,0,1\r\n";
    uint8_t buf[32], i = 0, ip;
    uint16_t port;
    
    
    return true;
    /* The following should be taken care of upon waking the modem: if defaults
     * are still programmed, don't try to connect for data
    for (uint8_t cond = 0; condition < 4; condition++)  // cond = "condition"
    {
         Read IP */
       /* ip = ee_read(0, IP1_ADDR + cond * 0x20);        // IP1, IP2, IP3, IP4
        
        if (ip == '#')
        {
            //LED_PATTERN = STILL_DEFAULT;
            return false;                               // Default still programmed
        }
        
    }*/
}