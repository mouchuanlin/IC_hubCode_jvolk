

#include "config.h"
#include "modem.h"
#include "uart.h"


void send_data()
{
    check_md_state();
}


void check_md_state()
{
    uint8_t tempStatus = 0;
    switch (MD_STATE)
    {
        case MD_OFF:
            MD_PWR = OFF;
            break;
        case WAITING:
            if (tmr5_cnt >= _T5_45s && !modem_awake)        // 4ticks per sec, 28-30 secs to boot
            {
                disable_tmr5();
                tempStatus = startup_ok();      // err code reporting; 1 = ok; 5 = timed out
                if (tempStatus == 1)            // started up ok
                {
                    
                    md_timeout_cnt = 0;
                    modem_awake = true;
                    MD_STATE = AWAKE_IDLE;
                }
                else if (tempStatus == 2)       // startup_ok() function still being processed
                    NOP();
                else if (tempStatus == 3)       // startup_ok timed out
                    md_timeout_cnt++;
            }
            else if (modem_awake)
            {
                
            }
            break;
        case AWAKE_IDLE:
            break;
        case CONNECTED:
            break;
        case RETRY_IP:
            // wait for timer to time out: 
            // for now, do retries back-to-back
            // and use long-term timers only for 
            // -SRECV repeats
            // -SGACT
            // 
            // The remaining AT cmds can be done with shorter timers
            break;
    }
}


void start_modem()
{
    check_md_state();
    if (MD_STATE != WAITING || (MD_STATE == WAITING && md_timeout_cnt >= 2))    // restart
    {
        MD_PWR = OFF;
        // would use modem reset here but transistor not currently installed
        // on board.
        __delay_ms(200);
        MD_PWR = ON;
        MD_STATE = WAITING;
        modem_awake = false;
        md_timeout_cnt = 0;
        // NEED:
//        start_md_boot_timer();
        numUartRetries = 15;
    }
    else if (MD_STATE == WAITING && (md_timeout_cnt == 1))
    {
        MD_STATE = WAITING;
        start_md_boot_timer();      // another 25s
        numUartRetries = 15;
    }
}


uint8_t startup_ok()
{
    uint8_t at[] =                "AT\r\n";
    uint8_t ate[] =             "ATE1\r\n";       // Turn on echo
    uint8_t cmee[] =       "AT+CMEE=2\r\n";       // Turn on verbose err msgs
    uint8_t baud[] =   "AT+IPR=115200\r\n";       // Set baud to 115.2k
    uint8_t ledCfg[] = "AT#GPIO=7,0,2\r\n";       // Config LED on GPIO
    uint8_t ledpattern[] = "AT#SLED=4\r\n";       // Config LED pattern
    uint8_t queryVersion[] = "AT#CGMR\r\n";       // Get module SW version
    uint8_t cpin[] =        "AT+CPIN?\r\n";       // Check SIM
    uint8_t creg[] =        "AT+CREG?\r\n";       // Check network registration
    uint8_t cereg[] =      "AT+CEREG?\r\n";       // Check network registration
    uint8_t gprs[] =        "AT#GPRS?\r\n";       //
    uint8_t csq[] =           "AT+CSQ\r\n";       // Check signal quality

    
    init_md_uart();
    
//    reload_tmr5_pt25s();
    if (at1 != 1)
    {
        at1 = md_uart_write_str_ok((uint8_t *)&at[0], sizeof(at), YES, NO);
        if (at1 == 1)
            numUartRetries = 15;
    }
    if (at1 == 1 && at2 != 1)
    {
        at2 = md_uart_write_str_ok((uint8_t *)&ate[0], sizeof(ate), YES, NO);
        if (at2 == 1)
            numUartRetries = 15;
    }
    if (at2 == 1 && at3 != 1)
    {
        at3 = md_uart_write_str_ok((uint8_t *)&cmee[0], sizeof(cmee), YES, NO);
        if (at3 == 1)
            numUartRetries = 15;
    }
    if (at3 == 1 && at4 != 1)
    {
        at4 = md_uart_write_str_ok((uint8_t *)&baud[0], sizeof(baud), YES, NO);
        if (at4 == 1)
            numUartRetries = 15;
    }
    if (at4 == 1 && at5 != 1)
    {
        at5 = md_uart_write_str_ok((uint8_t *)&ledCfg[0], sizeof(ledCfg), YES, NO);
        if (at5 == 1)
            numUartRetries = 15;
    }
    if (at5 == 1 && at6 != 1)
    {
        at6 = md_uart_write_str_ok((uint8_t *)&ledpattern[0], sizeof(ledpattern), YES, NO);
        if (at6 == 1)
            numUartRetries = 15;
    }
    if (at6 == 1 && at7 != 1)
    {
        at7 = md_uart_write_str_ok((uint8_t *)&queryVersion[0], sizeof(queryVersion), YES, NO);
        if (at7 == 1)
            numUartRetries = 15;
    }
    if (at7 == 1 && at8 != 1)
    {
        at8 = md_uart_write_str_ok((uint8_t *)&cpin[0], sizeof(cpin), YES, NO);
        if (at8 == 1)
            numUartRetries = 30;// 10 each for creg, cereg, csq
    }
    
    // creg should return 0,1 or 0,3; csq should return some number >0 and < 99 in first parameter
    if (at8 == 1 && (at9 != 1 || at10 != 1 || at11 != 1))
    {
        if (at9 != 1)
        {
            at9 = md_uart_write_str_ok((uint8_t *)&creg[0], sizeof(creg), YES, NO);
            if (at9 == 1)
                at9 = parse_creg_ok();
        }
        if (at10 != 1)
        {
            at10 = md_uart_write_str_ok((uint8_t *)&cereg[0], sizeof(cereg), YES, NO);
            if (at10 == 1)
                at10 = parse_cereg_ok();
        }
        if (at11 != 1)
        {
            at11 = md_uart_write_str_ok((uint8_t *)&csq[0], sizeof(csq), YES, NO);
            if (at11 == 1)
                at11 = parse_csq_ok();
        }

        if (at9 == 1 && at10 == 1 && at11 == 1)
            numUartRetries = 15;
    }
//    if (at10 == 1 && at11 != 1)
//    {
//        at11 = md_uart_write_str_ok((uint8_t *)&gprs[0], sizeof(gprs), YES, NO);
//    }
    
    if (at1 == 5 || at2 == 5 || at3 == 5 || at4 == 5 || at5 == 5 || at6 == 5 || \
            at7 == 5 || at8 == 5 || at9 == 5 || at10 == 5 || at11 == 5)// || at12 == 5)
    {
        clear_at_ok();
        return 5;       // "timed out" event code
    }
    else if (at1 == 1 && at2 == 1 && at3 == 1 && at4 == 1 && at5 == 1 && at6 == 1 && \
            at7 == 1 && at8 == 1 && at9 == 1 && at10 == 1 && at11 == 1 && at12 == 1)
    {
        clear_at_ok();
        return 1;
    }
    return 2;
}


void start_md_boot_timer()
{
    reload_tmr5_pt25s();
}