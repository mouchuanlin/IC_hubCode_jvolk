/* 
 * File:   modem.h
 * Author: Scott
 *
 * Created on September 14, 2018, 9:19 PM
 */

#ifndef MODEM_H
#define	MODEM_H

// INCLUDES
#include <stdbool.h>
#include <stdint.h>

// DEFINES
#define MODEM_TYPE              "LE866-SV1"
#define SMS_RETRY_CNT           5
#define APN_ADDR                0x10        // 35#
#define IP1_ADDR                0x30        // 01#
#define IP2_ADDR                0x50        // 02#
#define IP3_ADDR                0x70        // 03#
#define IP4_ADDR                0x90        // 04#
#define PORT1_ADDR              0xB0        // 31#
#define PORT2_ADDR              0xB2        // 32#
#define PORT3_ADDR              0xB4        // 33#
#define PORT4_ADDR              0xB6        // 34#
#define SMS_WAIT_TIME_ADDR      0xB8        // 09#, In minutes
#define ACCESS_CODE_ADDR        0xC0        // 05#
#define PROG_ACK_ADDR           0xC7        // 06#
#define TEST_FREQ_ADDR          0xC8        // 07#, In # of days
#define SERVER_ACK_TIME_ADDR    0xC9        // 08#
#define UNIT_ACCT_ADDR          0xCA        // 10#
#define LINE_CARD_ADDR          0xD0        // 11#
#define HUB_ZONE_ADDR           0xB9        // 12#
#define TP_PIN_EN_ADDR          0xBB        // 14#
#define NUM_NET_RETRY_ADDR      0xBC        // 15#
#define NUM_IP_RETRY_ADDR       0xBD        // 16#
#define ENCRYPTION_ADDR         0xE0        // 95#
#define NUM_SENSORS_ADDR        0x02
#define ID_START_ADDR           0x80        // on page 1 of eeprom


// VARYABLES
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


// FUNCTION PROTOS
void start_md_boot_timer();
void check_sms_timer();
void start_modem();
uint8_t startup_ok();
void send_data();
void check_md_state();
uint8_t parse_cmgl();
uint8_t parse_cmgr();

bool md_timer_up = false;
bool modem_awake = false;
bool modem_ready_sms = false;
bool modem_ready_data = false;
bool modem_network_err = false;
bool modem_retrying = false;
bool receivedOK, receivedERR, receivedRESP, timedOut;
bool waiting_uart = false, waiting_carrot = false;

uint8_t at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12;     // max 12 at cmds per function

uint8_t retryCnt = 0, md_timeout_cnt = 0;
uint8_t endPtr = 0;
uint8_t referenceBuf[60];
uint8_t sms_buf[40];

#endif	/* MODEM_H */

