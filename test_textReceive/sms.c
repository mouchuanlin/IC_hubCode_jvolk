/* 
 * File:   sms.c
 * Author: Jennifer
 * 
 * This file encompasses all functions related to SMS reception and sending.
 *
 * Created on April 11, 2018, 11:01 PM
 */
#include "sms.h"
#include "uart.h"
#include "eeprom.h"



bool send_text(uint8_t *msg, uint8_t len, uint8_t *number)
{
    md_uart_write_str_ok((uint8_t *)"AT+CMGF=1\r\n", 11, YES);
    
    delay5ms(2);
    md_uart_write_str_ok((uint8_t *)"AT+CMGS=", 8, NO);
    md_uart_write_str_ok(number, 10, NO);
    md_uart_write_str_ok((uint8_t *)"\r\n", 2, NO);
    
    uint8_t buf[40],temp, i = 0, count = 15, ctrlz = 26;
    bool ready_to_write = false;
//    CREN1 = 0;
    RC1IE = 0;
    CREN1 = 1;
    while (count-- != 0 && !ready_to_write)
    { 
        setup_timer3();
        while(!TMR3IF && !ready_to_write)
        {
			if( RC1IF == 1 )
			{                /* Re-think the following check: store in buffer, and then look for the words "ok" and "error"  */
				temp = RC1REG;
                buf[i++] = temp;
                
                if (i >= 20)
                    i = 19;
                
                    if (buf[0] == '>')
                    {
                        RC1IE = 1;
                        CREN1 = 0;
                        ready_to_write = true;
                    }
                    i = 0;
                
			}
		}
		TMR3IF = 0;
	}
    TMR3ON = 0;
    RC1IE = 1;
    CREN1 = 0;
    
    if (ready_to_write)
    {
        md_uart_write_str_ok(msg, len, NO);
        if (md_uart_write_str_ok((uint8_t *)&ctrlz, 1, YES))
            return true;
        else
            return false;
    }
    else
        return false;
    
}

bool receive_sms_init()
{
    unsigned char const cmgf[] = "AT+CMGF=1\r\n$";
    unsigned char const cpms[]="AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n$";
	unsigned char const cnmi[]="AT+CNMI=1,1\r\n$";
    uint8_t dummyBuf[40];
    while (!md_uart_write_str_ok((uint8_t *)&cmgf[0], sizeof(cmgf), YES))
        delay5ms(200);
    while (!md_uart_write_str_ok((uint8_t *)&cpms[0], sizeof(cpms), YES))
        delay5ms(200);
    while (!md_uart_write_str_ok((uint8_t *)&cnmi[0], sizeof(cnmi), YES))
        delay5ms(100);
    
    ledPattern = INTERNET;
    return true;
}

bool receive_sms()
{
    uint8_t sms_time = ee_read(0x00,SMS_WAIT_TIME_ADDR);   //wait SMS time
    uint8_t cnt = 60;
    while (sms_time-- > 0)          // sms_time * 1min = total time spent here
    {
        while (cnt-- > 0)           // 60 cycles ea (1min))
        {
            delay5ms(200);          // 1s ea
            check_sms();
        }
    }
    return true;
}


void check_sms()
{
    const uint8_t cmgl[] = "AT+CMGL=\"ALL\"\r\n";
    const uint8_t cmgl_check[] = "+CMGL: ";
    uint8_t msgPlace1, msgPlace2, msgPlace3, temp;

    while (!md_uart_write_str_ok((uint8_t *)&cmgl[0], sizeof(cmgl), YES));
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i ++)
    {
        if (str_matches((uint8_t *)&mdRxBuf[i], (uint8_t *)&cmgl_check[0], sizeof(cmgl_check)) && msgPlace1 == 0 && msgPlace2 == 0 && msgPlace3 == 0)
        {
            msgPlace1 = mdRxBuf[(uint8_t)(i + 7)];
            msgPlace2 = mdRxBuf[(uint8_t)(i + 8)];
            msgPlace3 = mdRxBuf[(uint8_t)(i + 9)];
        }
        else if (mdRxBuf[(uint8_t)(i - 1)] == 'O' && mdRxBuf[i] == 'K')
        {
            if (msgPlace1 == 0 && msgPlace2 == 0){}
            temp = read_sms(msgPlace1, msgPlace2, msgPlace3);
            delete_sms(msgPlace1, msgPlace2, msgPlace3);
        }
    }
}


uint8_t read_sms(uint8_t txtAddr1, uint8_t txtAddr2, uint8_t txtAddr3)
{
    const uint8_t cmgr[] = "AT+CMGR=";
    
    md_uart_write_str_ok((uint8_t *)&cmgr[0], sizeof(cmgr), NO);
    md_uart_write_str_ok((uint8_t *)(&txtAddr1), 1, NO);
    md_uart_write_str_ok((uint8_t *)(&txtAddr2), 1, NO);
    md_uart_write_str_ok((uint8_t *)(&txtAddr3), 1, NO);
    md_uart_write_str_ok((uint8_t *)"\r\n", 2, YES);
    
    return 0;
}

void delete_sms(uint8_t p1, uint8_t p2, uint8_t p3)
{
    const uint8_t cmgd[] = "AT+CMGD=";
    md_uart_write_str_ok((uint8_t *)&cmgd[0], sizeof(cmgd), NO);
    md_uart_write_str_ok((uint8_t *)(p1), 1, NO);
    if (p2 != 0)
        md_uart_write_str_ok((uint8_t *)(p2), 1, NO);
    if (p3 != 0)
        md_uart_write_str_ok((uint8_t *)(p3), 1, NO);
    md_uart_write_str_ok((uint8_t *)"\r\n", 2, YES);
}