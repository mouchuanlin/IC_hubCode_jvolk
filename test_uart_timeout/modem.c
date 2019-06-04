
#include <string.h>
#include "config.h"
#include "uart.h"
#include "modem.h"
    
static bool sentAT1 = false, sentAT2 = false, sentAT3 = false, sentAT4 = false, sentAT5 = false;
static bool sentAT6 = false, sentAT7 = false, sentAT8 = false, sentAT9 = false, sentAT10 = false;
static bool sentAT11 = false, sentAT12 = false;
static bool extendedTimer = false;


void start_modem_send_process(uint8_t newEvent)
{
    if (!alreadyAwake && md_state != IDLE)       // if coming from any other state
    {
        MD_POWER = OFF;
        // Delay 5s
        for (uint8_t i = 0; i < 8; i++)         // minimum 1.5s delay before re-powering
        {
            CLRWDT();
            __delay_ms(200);
        }
        CLRWDT();
    }
    else if (md_state != IDLE)
    {
        md_state = WAITING_FOR_BOOT;
    }
//    MD_POWER = ON;
//    start_md_boot_timer();
//    md_state = WAITING_FOR_BOOT;
    
    // FOR DEBUG ONLY, SKIP WAITING PERIOD
    md_state = CONFIG;
    
//    if (newEvent != 0x00 && !alreadyInQueue(newEvent))
//        add_event_to_queue(newEvent);
}

void start_md_boot_timer()
{
//    reload_tmr5_pt25s();
    reload_tmr3_10ms();
}

void check_modem_state()
{
    switch (md_state)
    {
        case IDLE:
            break;
        case WAITING_FOR_BOOT:
            if (bootTimerIsDone)
                md_state = CONFIG;
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
            send_over_socket_ok();
            while (!close_socket_ok());
            pdpIsActive = false;
            md_state = IDLE;
            MD_POWER = OFF;
            alreadyAwake = false;
            break;
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
    
    init_md_uart();
    
//    reload_tmr5_pt25s();
    if (!sentAT1)
        sentAT1 &= md_uart_write_str_ok((uint8_t *)&at[0], sizeof(at), YES, NO);
    if (sentAT1 && receivedOK && !sentAT2)
    {
        receivedOK = false;
        sentAT2 &= md_uart_write_str_ok((uint8_t *)&ate[0], sizeof(ate), YES, NO);
    }
    if (sentAT2 && receivedOK && !sentAT3)
    {
        receivedOK = false;
        sentAT3 &= md_uart_write_str_ok((uint8_t *)&cmee[0], sizeof(cmee), YES, NO);
    }
    if (sentAT3 && receivedOK  && !sentAT4)
    {
        receivedOK = false;
        sentAT4 &= md_uart_write_str_ok((uint8_t *)&baud[0], sizeof(baud), YES, NO);
    }
    if (sentAT4 && receivedOK  && !sentAT5)
    {
        receivedOK = false;
        sentAT5 &= md_uart_write_str_ok((uint8_t *)&ledCfg[0], sizeof(ledCfg), YES, NO);
    }
    if (sentAT5 && receivedOK  && !sentAT6)
    {
        receivedOK = false;
        sentAT6 &= md_uart_write_str_ok((uint8_t *)&ledpattern[0], sizeof(ledpattern), YES, NO);
    }
    if (sentAT6 && receivedOK  && !sentAT7)
    {
        receivedOK = false;
        sentAT7 &= md_uart_write_str_ok((uint8_t *)&queryVersion[0], sizeof(queryVersion), YES, NO);
    }
    if (sentAT7 && receivedOK  && !sentAT8)
    {
        receivedOK = false;
        sentAT8 &= md_uart_write_str_ok((uint8_t *)&cpin[0], sizeof(cpin), YES, NO);
    }
    if (sentAT8 && receivedOK  && !sentAT9)
    {
        receivedOK = false;
        sentAT9 &= md_uart_write_str_ok((uint8_t *)&creg[0], sizeof(creg), YES, NO);
    }
    if (sentAT9 && receivedOK  && !sentAT10)
    {
        receivedOK = false;
        sentAT10 &= md_uart_write_str_ok((uint8_t *)&cereg[0], sizeof(cereg), YES, NO);
    }
    if (sentAT10 && receivedOK  && !sentAT11)
    {
        receivedOK = false;
        sentAT11 &= md_uart_write_str_ok((uint8_t *)&gprs[0], sizeof(gprs), YES, NO);
    }
    if (sentAT11 && receivedOK  && !sentAT12)
    {
        receivedOK = false;
        sentAT12 &= md_uart_write_str_ok((uint8_t *)&csq[0], sizeof(csq), YES, NO);
    }
    
    
    if (!receivedOK && timedOut && !extendedTimer) // to cover all AT command errs
    {
        extendedTimer = true;
        timedOut = false;
        reset_AT_marks();
        start_md_boot_timer();      // wait another 25s if any cmd still isn't acknowledged OK...or make this timer shorter???
        md_state = WAITING_FOR_BOOT;
    }
    else if (!receivedOK && timedOut && extendedTimer)      // if, after the 2nd 25s period, the unit still gets an ERROR or no response,
    {
        extendedTimer = false;
        reset_AT_marks();
        start_modem_send_process(0x00);                     // Start again with NULL command
    }
    
    if (sentAT12 && receivedOK)
    {
        reset_AT_marks();
        alreadyAwake = true;
        receivedOK = false;
    }
    
    terminate_md_uart();
    return alreadyAwake;
}

// CASE: SET_UP_DATA
bool setup_data_ok()
{
    uint8_t setApn[] = "AT+CGDCONT=3,\"IPV4V6\",\"wyleslte.gw7.vzwentp\"\r\n";
    uint8_t cfgSocket[] = "AT#SCFGEXT=1,1,0,0,0,0\r\n";       // Will need to receive msgs with SRECV
    uint8_t activatePDP[] = "AT#SGACT=3,1\r\n";
    
    init_md_uart();
    
//    reload_tmr5_pt25s();
    if (!sentAT1)
    {
        receivedOK = false;
        sentAT1 &= md_uart_write_str_ok((uint8_t *)&setApn[0], sizeof(setApn), YES, NO);
    }
    if (sentAT1 && receivedOK && !sentAT2)
    {
        receivedOK = false;
        md_uart_write_str_ok((uint8_t *)&cfgSocket[0], sizeof(cfgSocket), YES, NO);
    }
    if (sentAT2 && receivedOK && !sentAT3)
    {
        receivedOK = false;
        pdpIsActive = md_uart_write_str_ok((uint8_t *)&activatePDP[0], sizeof(activatePDP), YES, NO);
    }
    
    if (sentAT3 && receivedOK)
    {
        pdpIsActive = true;
        reset_AT_marks();
        receivedOK = false;
    }
    terminate_md_uart();
//    md_state = WAITING_PDP_ACTIVE;
//    md_state = SENDING_DATA;
    return true;
}

// CASE: 
bool send_over_socket_ok()
{
    const uint8_t openConnection[] = "AT#SD=1,0,10510,\"modules.telit.com\",0,0,1\r\n";
    const uint8_t ssend[] = "AT#SSEND=1\r\n";//AT#SSENDEXT=1,";
    const uint8_t srecv[]="AT#SRECV=1,89\r\n";//prev. 1,200
    const uint8_t msg[] = "Echo.\r\n";// Server TCP connection on Socket #1. Receive with #SRECV command.\r\n";

    uint8_t terminateData = 26;
    init_md_uart();
    
    md_uart_write_str_ok((uint8_t *)&openConnection[0], sizeof(openConnection), YES, NO);
    md_uart_write_str_ok((uint8_t *)&ssend[0], sizeof(ssend), NO, YES);
    md_uart_write_str_ok((uint8_t *)&msg[0], sizeof(msg), NO, NO);
    // GETS STUCK AFTER THE FOLLOWING LINE WHILE WAITING FOR 'OK'
    md_uart_write_str_ok((uint8_t *)&terminateData, 1, YES, NO);
    CREN1 = 1;
    __delay_ms(2000);
    CREN1 = 0;
    md_uart_write_str_ok((uint8_t *)&srecv[0], sizeof(srecv), YES, NO);
    terminate_md_uart();
//    strcpy(ssend, "AT#SSENDEXT=1,");
//    strcat(ssend,)
    return true;
}


bool close_socket_ok()
{
    const uint8_t closeSocket[] = "AT#SH=1\r\n";
    const uint8_t deactivatePDP[] = "AT#SGACT=3,0\r\n";
    
    init_md_uart();
    
    md_uart_write_str_ok((uint8_t *)&closeSocket[0], sizeof(closeSocket), YES, NO);
    md_uart_write_str_ok((uint8_t *)&deactivatePDP[0], sizeof(deactivatePDP), YES, NO);
    terminate_md_uart();
    return true;
}

void reset_AT_marks()
{
    sentAT1 = false;
    sentAT2 = false;
    sentAT3 = false;
    sentAT4 = false;
    sentAT5 = false;
    sentAT6 = false;
    sentAT7 = false;
    sentAT8 = false;
    sentAT9 = false;
    sentAT10 = false;
    sentAT11 = false;
    sentAT12 = false;
}