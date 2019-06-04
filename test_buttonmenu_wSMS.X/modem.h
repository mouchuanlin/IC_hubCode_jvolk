/* 
 * File:   modem.h
 * Author: JV
 *
 * Created on May 4, 2018, 11:37 AM
 */

#ifndef MODEM_H
#define	MODEM_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"
#include "cell_settings.h"
#include "uart.h"


/*****************************************************
 * INCLUDES
 ****************************************************/
#define SMS_RETRY_CNT           5


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void start_modem();
bool startup_modem_ok();
void restart_modem();
void delay5ms(unsigned int cnt);
bool wakeup_modem_ok();
//bool setup_modem_io_ok();
bool init_modem_io_ok();
//bool sim_card_ok();
void check_sms_timer();
uint8_t startup_ok();
uint8_t parse_cmgl();
uint8_t parse_cmgr();


void start_md_boot_timer();
void check_md_state();




/*****************************************************
 * VARYABLES
 ****************************************************/
enum SMSTIMERSTATE {
    SMS_OFF,
    RUNNING,
    NEXT_SMS_READY,
    LISTEN_MODE_DONE
};
enum SMSTIMERSTATE SMS_TMR_STATE;

enum MODEMSTATE {
    MD_OFF,
    WAITING,        // in boot, waiting for AT to register
    AWAKE_IDLE,     // ready on your command
    CONNECTED,
    RETRY_IP,       // go into if IP1 doesn't work; try on IP2 (only if it is programmed in)
//    NETWORK_ERR
};
enum MODEMSTATE MD_STATE;


bool modem_awake = false;
bool modem_ready_sms = false;
bool receivedOK, receivedERR, receivedRESP, timedOut;
bool waiting_uart = false, waiting_carrot = false;

uint8_t retryCnt = 0, md_timeout_cnt = 0;
uint8_t at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12;     // max 12 at cmds per function
uint8_t referenceBuf[60];
uint8_t sms_buf[40];

#endif	/* MODEM_H */

