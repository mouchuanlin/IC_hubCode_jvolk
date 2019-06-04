/* 
 * File:   sms.c
 * Author: Jennifer
 * 
 * This file encompasses all functions related to SMS reception and sending.
 *
 * Created on April 11, 2018, 11:01 PM
 */
#include "sms.h"

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