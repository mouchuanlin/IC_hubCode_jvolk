
#include <string.h>
#include "config.h"
#include "uart.h"
#include "modem.h"
    

void start_modem_send_process()
{
    if (md_state != IDLE)       // if coming from any other state
    {
        MD_POWER = OFF;
        __delay_ms(5000);       // Note this delay!
    }
    MD_POWER = ON;
    start_md_boot_timer();
    md_state = WAITING_FOR_BOOT;
}

void start_md_boot_timer()
{
    reload_tmr5_pt25s();
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
            if (!send_md_cfg_cmds_ok())
            {
                start_modem_send_process();             // essentially a re-boot
                md_state = WAITING_FOR_BOOT;
            }
            break;
        case SET_UP_DATA:
            setup_data_ok();// && dataSetupRetries > 0)
            md_state = SENDING_DATA;
            break;
        case WAITING_PDP_ACTIVE:
            if (pdpIsActive)
                md_state = SENDING_DATA;
            break;
        case SENDING_DATA:
//            for (uint8_t i = 0; i < 5; i ++)                // simulate LED control here
//            {
//                B_ON();
//                __delay_ms(100);
//                B_OFF();
//                __delay_ms(100);
//            }
            send_over_socket_ok();
            while (!close_socket_ok());
            pdpIsActive = false;
            md_state = IDLE;
            MD_POWER = OFF;
            break;
//            if (timedOut)
//            {
//                md_state = SET_UP_DATA;
//                dataSetupRetries--;
//            }
    }
}


bool send_md_cfg_cmds_ok()
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
    //    const uint8_t checkSIM[]    = "AT+CPIN?\r\n";       //** note need to check for "READY" in this case

    bool gotOk = true;
    init_md_uart();
    
//    reload_tmr5_pt25s();
    gotOk &= md_uart_write_str_ok((uint8_t *)&at[0], sizeof(at), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&ate[0], sizeof(ate), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&cmee[0], sizeof(cmee), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&baud[0], sizeof(baud), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&ledCfg[0], sizeof(ledCfg), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&ledpattern[0], sizeof(ledpattern), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&queryVersion[0], sizeof(queryVersion), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&cpin[0], sizeof(cpin), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&creg[0], sizeof(creg), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&cereg[0], sizeof(cereg), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&gprs[0], sizeof(gprs), YES, NO);
    gotOk &= md_uart_write_str_ok((uint8_t *)&csq[0], sizeof(csq), YES, NO);
    terminate_md_uart();
    
//    if (!queueIsEmpty && hub_state == IDLE)              // If there's anything in queue,
        md_state = SET_UP_DATA;     // send it out first.
//    else if (hub_state == SMS_LISTEN)           // Otherwise if the radio's not woken to send something out,
//        md_state = SET_UP_SMS;      // assume that it's woken up to go into SMS listen mode.
    return true;
}


bool setup_data_ok()
{
    uint8_t setApn[] = "AT+CGDCONT=3,\"IPV4V6\",\"wyleslte.gw7.vzwentp\"\r\n";
    uint8_t cfgSocket[] = "AT#SCFGEXT=1,1,0,0,0,0\r\n";       // Will need to receive msgs with SRECV
    uint8_t activatePDP[] = "AT#SGACT=3,1\r\n";
    
    init_md_uart();
    
//    reload_tmr5_pt25s();
    md_uart_write_str_ok((uint8_t *)&setApn[0], sizeof(setApn), YES, NO);
    md_uart_write_str_ok((uint8_t *)&cfgSocket[0], sizeof(cfgSocket), YES, NO);
    pdpIsActive = md_uart_write_str_ok((uint8_t *)&activatePDP[0], sizeof(activatePDP), YES, NO);
    terminate_md_uart();
    pdpIsActive = true;
//    md_state = WAITING_PDP_ACTIVE;
//    md_state = SENDING_DATA;
    return true;
}

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