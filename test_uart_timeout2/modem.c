
#include <string.h>
#include "config.h"
#include "uart.h"
#include "modem.h"
    
static uint8_t sentAT1 = 0, sentAT2 = 0, sentAT3 = 0, sentAT4 = 0, sentAT5 = 0;
static uint8_t sentAT6 = 0, sentAT7 = 0, sentAT8 = 0, sentAT9 = 0, sentAT10 = 0;
static uint8_t sentAT11 = 0, sentAT12 = 0;
static bool extendedTimer = false;


void start_modem_send_process(uint8_t newEvent)
{
//    if (newEvent != 0x00 && !alreadyInQueue(newEvent))
//        add_event_to_queue(newEvent);// and set queueIsEmpty to false
    if (!awake && newEvent == 0x00)//&& md_state == CONFIG)       // if coming from any other state
    {
        start_md_boot_timer();
        md_state = WAITING_FOR_BOOT;
        MD_POWER = OFF;
        for (uint8_t i = 0; i < 10; i++)         // minimum 1.5s delay before re-powering
        {
            CLRWDT();
            __delay_ms(200);
        }
        CLRWDT();
    }
//    else if (awake && md_state == WAITING_FOR_BOOT )
//    {
//        md_state = CONFIG;
//    }
    MD_POWER = ON;
    start_md_boot_timer();
    md_state = WAITING_FOR_BOOT;
    
    // FOR DEBUG ONLY, SKIP WAITING PERIOD
//    md_state = CONFIG;
    
}

void start_md_boot_timer()
{
//    tmr3_cnt = 0;
    reload_tmr3_1s();
}

void check_modem_state()
{
    switch (md_state)
    {
        case IDLE:
            break;
        case WAITING_FOR_BOOT:
            if (bootTimerIsDone)
            {
                bootTimerIsDone = false;
                numRetries = 15;
                md_state = CONFIG;
            }
            break;
        case CONFIG:
            if (send_md_cfg_cmds_ok())
            {
//                if (!queueIsEmpty)                                  // If there's anything in queue (regardless of current state),
                    md_state = WAITING_PDP_ACTIVE;                  // get ready to send data
//                else if (state == SMS_LISTEN)                       // Otherwise if the radio's not woken to send something out, 
//                    md_state = SET_UP_SMS;                          // assume it's to go into SMS listen mode
            }
            break;
        case WAITING_PDP_ACTIVE:
            setup_data_ok();// && dataSetupRetries > 0)
            if (pdpIsActive)
                md_state = SENDING_DATA;
            break;
        case SENDING_DATA:
            if (send_over_socket_ok())
            {
                md_state = CLOSE_SOCKET;
            }
            break;
        case CLOSE_SOCKET:
            if (close_socket_ok())
            {
                pdpIsActive = false;
                md_state = IDLE;
                MD_POWER = OFF;
                awake = false;
            }
//            if (timedOut)
//            {
//                md_state = SET_UP_DATA;
//                dataSetupRetries--;
//            }
    }
}

// CASE: CONFIG
bool send_md_cfg_cmds_ok()
{
    uint8_t at[] =                "AT\r\n";
    uint8_t ate[] =             "ATE1\r\n";       // Turn on echo
    uint8_t cmee[] =       "AT+CMEE=2\r\n";       // Turn on verbose err msgs
    uint8_t baud[] =   "AT+IPR=115200\r\n";       // Set baud to 115.2k
    uint8_t ledCfg[] = "AT#GPIO=7,0,2\r\n";       // Config LED on GPIO
    uint8_t ledpattern[] = "AT#SLED=4\r\n";       // Config LED pattern
    uint8_t queryVersion[] = "AT#CGMR\r\n";       // Get module SW version
    uint8_t cpin[] =        "AT+CPIN?\r\n";       // Check SIM; must validate "READY" value
    uint8_t creg[] =        "AT+CREG?\r\n";       // Check network registration; must validate return values
    uint8_t cereg[] =      "AT+CEREG?\r\n";       // Check network registration; must validate return values
    uint8_t gprs[] =        "AT#GPRS?\r\n";       //
    uint8_t csq[] =           "AT+CSQ\r\n";       // Check signal quality; must validate returned values
    //    const uint8_t checkSIM[]    = "AT+CPIN?\r\n";       //** note need to check for "READY" in this case
    
    
    if (!(sentAT1 == ERR_CODE_REC_OK))                  // waits for return status == 0x03
        sentAT1 = md_uart_write_str_ok((uint8_t *)&at[0], sizeof(at), YES, NO);
    if (sentAT1 == ERR_CODE_REC_OK && !(sentAT2 == ERR_CODE_REC_OK))
        sentAT2 = md_uart_write_str_ok((uint8_t *)&ate[0], sizeof(ate), YES, NO);
    if (sentAT2 == ERR_CODE_REC_OK && !(sentAT3 == ERR_CODE_REC_OK))
        sentAT3 = md_uart_write_str_ok((uint8_t *)&cmee[0], sizeof(cmee), YES, NO);
    if (sentAT3 == ERR_CODE_REC_OK && !(sentAT4 == ERR_CODE_REC_OK))
        sentAT4 = md_uart_write_str_ok((uint8_t *)&baud[0], sizeof(baud), YES, NO);
    if (sentAT4 == ERR_CODE_REC_OK && !(sentAT5 == ERR_CODE_REC_OK))
        sentAT5 = md_uart_write_str_ok((uint8_t *)&ledCfg[0], sizeof(ledCfg), YES, NO);
    if (sentAT5 == ERR_CODE_REC_OK && !(sentAT6 == ERR_CODE_REC_OK))
        sentAT6 = md_uart_write_str_ok((uint8_t *)&ledpattern[0], sizeof(ledpattern), YES, NO);
    if (sentAT6 == ERR_CODE_REC_OK && !(sentAT7 == ERR_CODE_REC_OK))
        sentAT7 = md_uart_write_str_ok((uint8_t *)&queryVersion[0], sizeof(queryVersion), YES, NO);
    if (sentAT7 == ERR_CODE_REC_OK && !(sentAT8 == ERR_CODE_REC_OK))
        sentAT8 = md_uart_write_str_ok((uint8_t *)&cpin[0], sizeof(cpin), YES, NO);
    if (sentAT8 == ERR_CODE_REC_OK && !(sentAT9 == ERR_CODE_REC_OK))
        sentAT9 = md_uart_write_str_ok((uint8_t *)&creg[0], sizeof(creg), YES, NO);
    if (sentAT9 == ERR_CODE_REC_OK && !(sentAT10 == ERR_CODE_REC_OK))
        sentAT10 = md_uart_write_str_ok((uint8_t *)&cereg[0], sizeof(cereg), YES, NO);
    if (sentAT10 == ERR_CODE_REC_OK && !(sentAT11 == ERR_CODE_REC_OK))
        sentAT11 = md_uart_write_str_ok((uint8_t *)&gprs[0], sizeof(gprs), YES, NO);
    if (sentAT11 == ERR_CODE_REC_OK && !(sentAT12 == ERR_CODE_REC_OK))
        sentAT12 = md_uart_write_str_ok((uint8_t *)&csq[0], sizeof(csq), YES, NO);
    
    if ((sentAT1 == ERR_CODE_RETRIES_ENDED || sentAT2 == ERR_CODE_RETRIES_ENDED || \
            sentAT3 == ERR_CODE_RETRIES_ENDED || sentAT4 == ERR_CODE_RETRIES_ENDED || \
            sentAT5 == ERR_CODE_RETRIES_ENDED || sentAT6 == ERR_CODE_RETRIES_ENDED || \
            sentAT7 == ERR_CODE_RETRIES_ENDED || sentAT8 == ERR_CODE_RETRIES_ENDED || \
            sentAT9 == ERR_CODE_RETRIES_ENDED || sentAT10 == ERR_CODE_RETRIES_ENDED || \
            sentAT11 == ERR_CODE_RETRIES_ENDED || sentAT12 == ERR_CODE_RETRIES_ENDED) && \
            !extendedTimer)            // If any have timed out and not into extendedTimer yet
    {
        extendedTimer = true;
        timedOut = false;
        reset_AT_marks();
        start_md_boot_timer();      // wait another 25s if any cmd still isn't acknowledged OK...or make this timer shorter???
//        md_state = IDLE;
        md_state = WAITING_FOR_BOOT;
    }
    else if ((sentAT1 == ERR_CODE_RETRIES_ENDED || sentAT2 == ERR_CODE_RETRIES_ENDED || \
            sentAT3 == ERR_CODE_RETRIES_ENDED || sentAT4 == ERR_CODE_RETRIES_ENDED || \
            sentAT5 == ERR_CODE_RETRIES_ENDED || sentAT6 == ERR_CODE_RETRIES_ENDED || \
            sentAT7 == ERR_CODE_RETRIES_ENDED || sentAT8 == ERR_CODE_RETRIES_ENDED || \
            sentAT9 == ERR_CODE_RETRIES_ENDED || sentAT10 == ERR_CODE_RETRIES_ENDED || \
            sentAT11 == ERR_CODE_RETRIES_ENDED || sentAT12 == ERR_CODE_RETRIES_ENDED) && \
            extendedTimer)      // if, after the 2nd 25s period, the unit still gets an ERROR or no response,
    {
        extendedTimer = false;
        reset_AT_marks();
        start_modem_send_process(0x00);                     // start again with NULL command
    }
    
    if (sentAT12 == ERR_CODE_REC_OK)
    {
        reset_AT_marks();
        awake = true;
    }
    else
        awake = false;
    
//    __delay_ms(10);
    return awake;
}

// CASE: SET_UP_DATA
bool setup_data_ok()
{
    uint8_t setApn[] = "AT+CGDCONT=3,\"IPV4V6\",\"wyleslte.gw7.vzwentp\"\r\n";
    uint8_t cfgSocket[] = "AT#SCFGEXT=1,1,0,0,0,0\r\n";       // Will need to receive msgs with SRECV
    uint8_t activatePDP[] = "AT#SGACT=3,1\r\n";
    
    
    if (!(sentAT1 == ERR_CODE_REC_OK))
        sentAT1 = md_uart_write_str_ok((uint8_t *)&setApn[0], sizeof(setApn), YES, NO);
    if ((sentAT1 == ERR_CODE_REC_OK) && !(sentAT2 == ERR_CODE_REC_OK))
        sentAT2 = md_uart_write_str_ok((uint8_t *)&cfgSocket[0], sizeof(cfgSocket), YES, NO);
    if ((sentAT2 == ERR_CODE_REC_OK) && !(sentAT3 == ERR_CODE_REC_OK))
        sentAT3 = md_uart_write_str_ok((uint8_t *)&activatePDP[0], sizeof(activatePDP), YES, NO);
    if (sentAT3 == ERR_CODE_REC_OK)
    {
        pdpIsActive = true;
        reset_AT_marks();
        return true;
    }
    if (sentAT1 == ERR_CODE_TIMED_OUT || sentAT2 == ERR_CODE_TIMED_OUT || \
            sentAT3 == ERR_CODE_TIMED_OUT)
    {
        /* Special case */
    }
//    md_state = WAITING_PDP_ACTIVE;
//    md_state = SENDING_DATA;
    return false;
}

// CASE: 
bool send_over_socket_ok()
{
    const uint8_t openConnection[] = "AT#SD=1,0,10510,\"modules.telit.com\",0,0,1\r\n";
    const uint8_t ssend[] = "AT#SSEND=1\r\n";//AT#SSENDEXT=1,";
    const uint8_t srecv[]="AT#SRECV=1,89\r\n";//prev. 1,200
    const uint8_t msg[] = "Echo.\r\n";// Server TCP connection on Socket #1. Receive with #SRECV command.\r\n";

    uint8_t terminateData = 26;
    
    if (!(sentAT1  == ERR_CODE_REC_OK))
        sentAT1 = md_uart_write_str_ok((uint8_t *)&openConnection[0], sizeof(openConnection), YES, NO);
    if ((sentAT1 == ERR_CODE_REC_OK) && !(sentAT2 == ERR_CODE_REC_OK))
        sentAT2 = md_uart_write_str_ok((uint8_t *)&ssend[0], sizeof(ssend), NO, YES);
    if ((sentAT2 == ERR_CODE_REC_OK) && !(sentAT3 == ERR_CODE_REC_OK))
        sentAT3 = md_uart_write_str_ok((uint8_t *)&msg[0], sizeof(msg), NO, NO);
    if ((sentAT3 == ERR_CODE_REC_OK) && !(sentAT4 == ERR_CODE_REC_OK))
        sentAT4 = md_uart_write_str_ok((uint8_t *)&terminateData, 1, YES, NO);
//    CREN1 = 1;
//    __delay_ms(2000);
//    CREN1 = 0;
    if ((sentAT4 == ERR_CODE_REC_OK) && !(sentAT5 == ERR_CODE_REC_OK))
        sentAT5 = md_uart_write_str_ok((uint8_t *)&srecv[0], sizeof(srecv), YES, NO);
    
    if (sentAT5 == ERR_CODE_REC_OK)
    {
        reset_AT_marks();
        return true;
    }
    if (sentAT1 == ERR_CODE_TIMED_OUT || sentAT2 == ERR_CODE_TIMED_OUT || \
            sentAT3 == ERR_CODE_TIMED_OUT || sentAT4 == ERR_CODE_TIMED_OUT || \
            sentAT5 == ERR_CODE_TIMED_OUT)
    {
        /* Special case */
    }
//    strcpy(ssend, "AT#SSENDEXT=1,");
//    strcat(ssend,)
    return false;
}


bool close_socket_ok()
{
    const uint8_t closeSocket[] = "AT#SH=1\r\n";
    const uint8_t deactivatePDP[] = "AT#SGACT=3,0\r\n";
    const uint8_t syshalt[] = "AT#FASTSYSHALT\r\n";// OR SYSHALT for slower execution
    
    if (!(sentAT1 == ERR_CODE_REC_OK))
        sentAT1 = md_uart_write_str_ok((uint8_t *)&closeSocket[0], sizeof(closeSocket), YES, NO);
    if ((sentAT1 == ERR_CODE_REC_OK) && !(sentAT2 == ERR_CODE_REC_OK))
        sentAT2 = md_uart_write_str_ok((uint8_t *)&deactivatePDP[0], sizeof(deactivatePDP), YES, NO);
    
    if ((sentAT2 == ERR_CODE_REC_OK) && !(sentAT3 == ERR_CODE_REC_OK))
        sentAT3 = md_uart_write_str_ok((uint8_t *)&syshalt[0], sizeof(syshalt), YES, NO);
    if (sentAT3 == ERR_CODE_REC_OK)
    {
        pdpIsActive = false;
        reset_AT_marks();
        return true;
    }
    if (sentAT1 == ERR_CODE_TIMED_OUT || sentAT2 == ERR_CODE_TIMED_OUT || sentAT3 == ERR_CODE_TIMED_OUT)
    {
        /* Special case */
    }
    return false;
}

void reset_AT_marks()
{
    sentAT1 = 0;
    sentAT2 = 0;
    sentAT3 = 0;
    sentAT4 = 0;
    sentAT5 = 0;
    sentAT6 = 0;
    sentAT7 = 0;
    sentAT8 = 0;
    sentAT9 = 0;
    sentAT10 = 0;
    sentAT11 = 0;
    sentAT12 = 0;
}

void reset_retry_count()
{
    switch (md_state)
    {
        // Not relevant for any states before CONFIG
        case CONFIG:
            numRetries = 15;
            break;
        case WAITING_PDP_ACTIVE:
            numRetries = 20;
            break;
        case SENDING_DATA:          // Including SRECV command
            numRetries = 20;
            break;
        case CLOSE_SOCKET:
            numRetries = 15;
            break;
        case SET_UP_SMS:            // Including CPMS command
            numRetries = 15;
            break;
        case SENDING_SMS:
            numRetries = 15;
            break;
    }
}