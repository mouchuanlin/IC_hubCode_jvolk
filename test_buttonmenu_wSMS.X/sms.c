
#include "config.h"
#include "modem.h"
#include "state.h"
#include "uart.h"

uint8_t ph_num_index;
uint8_t ph_num[9];

void listen_sms()
{
    uint8_t start_ok = start_sms_ok();
    if (!modem_awake)
        start_modem();
    else if (modem_awake && !modem_ready_sms)
    {
        if (start_ok == 1)
        {
            start_sms_timer();      // interval between SMS reads, tracks total time in SMS listen mode
            modem_ready_sms = true;
        }
        else if (start_ok == 5)
            start_modem();
    }
    else if (modem_ready_sms)
    {
        if (sms_timer_ready && !sms_echo_ready)         // echo_ready is set when there's a message pending to be sent
        {
            retryCnt = 0;
            sms_timer_ready = false;
            read_sms();
        }
        if (sms_echo_ready)
        {
            if (send_sms_ok(true) == 1 && retryCnt++ < SMS_RETRY_CNT)              // times out after enough tries
            {
                retryCnt = 0;
                sms_echo_ready = false;
            }
            if (retryCnt >= SMS_RETRY_CNT)
            {
                retryCnt = 0;
                sms_echo_ready = false;          // give up
            }
        }
        else if (sms_err_ready)
        {
            if (send_sms_ok(false) == 1 && retryCnt++ < SMS_RETRY_CNT)
            {
                retryCnt = 0;
                sms_err_ready = false;
            }
            if (retryCnt >= SMS_RETRY_CNT)
                sms_err_ready = false;          // give up
        }
        check_sms_timer();
    }
}





uint8_t start_sms_ok()
{
    uint8_t cmgf[] = "AT+CMGF=1\r\n";
    uint8_t cpms[]="AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n";
	uint8_t cnmi[]="AT+CNMI=1,1\r\n";
    
    init_md_uart();
    
    if (at1 != 1)
    {
        at1 = md_uart_write_str_ok((uint8_t *)&cmgf[0], sizeof(cmgf), YES, NO);
        if (at1 == 1)
            numUartRetries = 15;
    }
    if (at1 == 1 && at2 != 1)
    {
        at2 = md_uart_write_str_ok((uint8_t *)&cpms[0], sizeof(cpms), YES, NO);
        if (at2 == 1)
            numUartRetries = 15;
    }
    if (at2 == 1 && at3 != 1)
    {
        at3 = md_uart_write_str_ok((uint8_t *)&cnmi[0], sizeof(cnmi), YES, NO);
        if (at3 == 1)
            numUartRetries = 15;
    }
    if (at1 == 5 || at2 == 5 || at3 == 5)
    {
        clear_at_ok();
        return 5;       // "timed out" event code
    }
    else if (at1 == 1 && at2 == 1 && at3 == 1)
    {
        clear_at_ok();
        return 1;
    }
    else
        return 2;
}


uint8_t read_sms()
{
    uint8_t cmgl[] = "AT+CMGL=\"ALL\"\r\n";
    uint8_t cmgr[] = "AT+CMGR=";
    uint8_t msgIndex[] = "\r\n";
    uint8_t cmgd[] = "AT+CMGD=";
    
    init_md_uart();
    
    
    numUartRetries = 15;    // retries only limited by duration of sms listen mode
        at1 = md_uart_write_str_ok((uint8_t *)&cmgl[0], sizeof(cmgl), YES, NO);
        if (at1 == 1)
        {
            if (parse_cmgl() == 1)      // contains more data to parse
            {
                ready_to_read_text = true;
            }
        }
        else if (at1 == 5)
        {
            clear_at_ok();
            return 5;
        }
        if (ready_to_read_text)
        {
            at2 = md_uart_write_str_ok((uint8_t *)&cmgr[0], sizeof(cmgr), NO, NO);
            at2 = md_uart_write_str_ok((uint8_t *)smsMsgIndex, 1, NO, NO);
            at2 = md_uart_write_str_ok((uint8_t *)&msgIndex[0], sizeof(msgIndex), YES, NO);

            if (at2 == 1)
            {
                if (parse_cmgr() == 1)      // valid command, was programmed in ok
                {
                    sms_echo_ready = true;
                    clear_at_ok();
                    ready_to_read_text = false;
                    return 1;
                }
                else
                {
                    sms_err_ready = true;
                    at3 = md_uart_write_str_ok((uint8_t *)&cmgd[0], sizeof(cmgd), NO, NO);
                    at3 = md_uart_write_str_ok((uint8_t *)smsMsgIndex, 1, NO, NO);
                    at3 = md_uart_write_str_ok((uint8_t *)&msgIndex[0], sizeof(msgIndex), YES, NO);
                    if (at3 == 1)
                    {
                        clear_at_ok();
                        ready_to_read_text = false;
                        return 4;
                    }
                    else if (at3 == 5)
                    {
                        clear_at_ok();
                        ready_to_read_text = false;
                        return 5;
                    }
                }
            }
            if (at2 == 5)
            {
                ready_to_read_text = false;
                return 5;
            }
            
    }
            return 2;
}


uint8_t send_sms_ok(bool isValidCmd)
{
    uint8_t cmgs[] = "AT+CMGS=";
    uint8_t msgIndex[] = "\r\n";
    uint8_t poundCount = 0, i = 0, smsEndIndex;
            
    init_md_uart();
    
    if (at1 != 1)
    {
        at1 = md_uart_write_str_ok((uint8_t *)&cmgs[0], sizeof(cmgs), NO, NO);
        at1 = md_uart_write_str_ok((uint8_t *)&ph_num[0], sizeof(ph_num), NO, NO);
        at1 = 0;
        at1 = md_uart_write_str_ok((uint8_t *)&msgIndex[0], sizeof(msgIndex), NO, YES);
        while (at1 != 1 && !timedOut);  // spin
        if (at1 == 1 && isValidCmd)
        {
            for (i = 0; i < sizeof(sms_buf); i++)
            {
                if (sms_buf[i] == '#')
                    poundCount ++;
                if (poundCount == 2)
                {
                    smsEndIndex = (uint8_t)(i + 1);
                    poundCount++;       // don't write over smsEndIndex again
                }
            }
                
            at2 = md_uart_write_str_ok((uint8_t *)&sms_buf[0], smsEndIndex, NO, NO);
            at2 = md_uart_write_str_ok((uint8_t *)(0x1A), 1, YES, NO);
            while (at2 != 1 && !timedOut)
                CLRWDT();
        }
        if (at1 == 1)
        {
            clear_at_ok();
            return 1;
        }
        else if (at1 == 4)
            return 4;
        else if (at1 == 5)
            return 5;
    }
    return 2;
    
}



uint8_t parse_cmgl()
{
    uint8_t rec_msg[] = "REC ";
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i++)
    {
        if (mdRxBuf[i] == rec_msg[0] && mdRxBuf[(uint8_t)(i + 1)] == rec_msg[1] && \
                mdRxBuf[(uint8_t)(i + 2)] == rec_msg[2] && mdRxBuf[(uint8_t)(i + 3)] == rec_msg[3])
        {
            smsMsgIndex = mdRxBuf[(uint8_t)(i - 3)];
            return 1;
        }
        else
            return 4;
    }
    return 4;
}

uint8_t parse_cmgr()
{
    uint8_t cmgr_rsp[] = "+CMGR: ";
    uint8_t access_code[4];
    uint8_t colon_index, newline_index;
    uint8_t field_num_tens, field_num_ones;
    uint8_t final_field_index = 0, final_field_term = 0;
    
    access_code[0] = ee_read(0x00, 0xC0);
    access_code[1] = ee_read(0x00, 0xC1);
    access_code[2] = ee_read(0x00, 0xC2);
    access_code[3] = ee_read(0x00, 0xC3);
    
    for (uint8_t i = 0; i < sizeof(mdRxBuf); i++)
    {
        if (mdRxBuf[i] == ':')
            colon_index = i;
        if (mdRxBuf[i] == ',' && mdRxBuf[(uint8_t)(i + 2)] == '+')
        {
            ph_num_index = (uint8_t)(i + 3);       // 9 digits long
            for (uint8_t l = 0; l < sizeof(ph_num); l++)
                ph_num[l] = mdRxBuf[(uint8_t)(ph_num_index + l)];      // store originating mobile number
        }
        if (mdRxBuf[i] == '\n')
            newline_index = i;
        if (mdRxBuf[i] == access_code[0] && mdRxBuf[(uint8_t)(i + 1)] == access_code[1] && \
                mdRxBuf[(uint8_t)(i + 2)] == access_code[2] && mdRxBuf[(uint8_t)(i + 3)] == access_code[3] && \
                mdRxBuf[(uint8_t)(i + 4)] == '#')
        {
            if (mdRxBuf[(uint8_t)(i + 7)] == '#')      // two digits
            {
                if (mdRxBuf[(uint8_t)(i + 5)] >= 0x41)
                    field_num_tens = (uint8_t)((mdRxBuf[(uint8_t)(i + 5)] - 0x41 + 0x0A) * 10);
                else
                    field_num_tens = (uint8_t)((mdRxBuf[(uint8_t)(i + 5)] - 0x30) * 10);
                if (mdRxBuf[(uint8_t)(i + 6)] >= 0x41)
                    field_num_ones = (uint8_t)(mdRxBuf[(uint8_t)(i + 6)] - 0x41 + 0x0A);
                else
                    field_num_ones = (uint8_t)(mdRxBuf[(uint8_t)(i + 6)] - 0x30);
                field_num_tens += field_num_ones;
                final_field_index = (uint8_t)(i + 8);
            }
            else if (mdRxBuf[(uint8_t)(i + 6)] == '#') // one digit
            {
                field_num_tens = 0;
                if (mdRxBuf[(uint8_t)(i + 5)] >= 0x41)
                    field_num_ones = (uint8_t)(mdRxBuf[(uint8_t)(i + 5)] - 0x41 + 0x0A);
                else
                    field_num_ones = (uint8_t)(mdRxBuf[(uint8_t)(i + 5)] - 0x30);
                field_num_tens += field_num_ones;
                final_field_index = (uint8_t)(i + 7);
            }
            else
                return 4;                   // err in format, or wrong access code
        }
    }
    for (uint8_t j = final_field_index; j < sizeof(mdRxBuf); j++)
    {
        if (mdRxBuf[j] == '#')
            final_field_term = j;
    }
    if (final_field_term == 0)
        return 4;
    for (uint8_t k = final_field_index; k < final_field_term; k++)
    {
        referenceBuf[(uint8_t)(k - final_field_index)];
    }
    if (program_field(field_num_tens, referenceBuf, (uint8_t)(final_field_term - final_field_index)))
        return 1;
    else
        return 4;
}


void check_sms_timer()
{
    uint8_t sms_duration = (uint8_t)(ee_read_ascii_to_value(0x00, SMS_WAIT_TIME_ADDR) * 20);// 20x 3s frames in one minute
    switch(SMS_TMR_STATE)
    {
        case OFF:
            break;
        case RUNNING:
            if (t2_tick >= _T2_3s)
            {
                t2_tick = 0;
                sms_tmr_tick++;         // how many 3s until you reach SMS listen mode timeout
                SMS_TMR_STATE = NEXT_SMS_READY;
            }
            if (sms_tmr_tick >= sms_duration)
            {
                sms_tmr_tick = 0;
                SMS_TMR_STATE = LISTEN_MODE_DONE;
            }
            break;
        case NEXT_SMS_READY:
            sms_timer_ready = true;
            SMS_TMR_STATE = RUNNING;
            break;
        case LISTEN_MODE_DONE:
            stop_sms_timer();
            modem_ready_sms = false;
            state = prev_state;
            SMS_TMR_STATE = OFF;
            break;
    }
}


void start_sms_timer()
{
    T2CONbits.T2CKPS = 0b10;        // 1:16 pre-scale
    T2CONbits.T2OUTPS = 0b1111;     // 1:16 post-scale
    T2CONbits.TMR2ON = 1;
    PIE1bits.TMR2IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void stop_sms_timer()
{
    T2CONbits.TMR2ON = 0;
    PIE1bits.TMR2IE = 0;
}