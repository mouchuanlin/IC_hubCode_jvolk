/* 
 * File:   default_settings.h
 * Author: JV
 *
 * Created on May 4, 2018, 12:18 PM
 */

#ifndef DEFAULT_SETTINGS_H
#define	DEFAULT_SETTINGS_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"
#include "eeprom.h"


/*****************************************************
 * DEFINES
 ****************************************************/
#define FIRST_RUN_CHECK                     0xF0        // EEPROM address corresp. to this value

// EEPROM Addresses for each parameter
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


#define VER_ADDR0                           0x00
#define VER_ADDR1                           0x01
#define VER_ADDR2                           0x02


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void write_defaults();
bool is_first_run();
void save_first_run();
bool program_field(uint8_t field_number, uint8_t data_buf[], uint8_t size);


/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t const default_APN[]="#";              //35#
uint8_t const default_IP1[]="#";              //01#
uint8_t const default_IP2[]="#";              //02#
uint8_t const default_IP3[]="#";              //03#
uint8_t const default_IP4[]="#";              //04#
unsigned int const default_PORT1 = 2020;            //31#   
unsigned int const default_PORT2 = 2020;            //32#   
unsigned int const default_PORT3 = 2020;            //33#   
unsigned int const default_PORT4 = 2020;            //34#   
uint8_t const default_ACCESS_CODE[]="1111#";  //05#
uint8_t const default_PROGRAM_ACK=0x01;       //06#
uint8_t const default_TEST_FREQ=15;           //07#
uint8_t const default_SERVER_ACK_TIME=45;     //08#
uint8_t const default_ACCT[]="0000#";   //10#
uint8_t const default_LINE_CARD[]="1#";       //11#
uint8_t const default_ZONE1=1;                //12#
uint8_t const default_ZONE2=20;               //13#
uint8_t const default_TP_PIN=0;               //14#
uint8_t const default_CYCLE = 2;              //15#
uint8_t const default_RETRY = 10;             //16#
uint8_t const default_SMS_WAIT_TIME = 10;     //09#
uint8_t const default_ENCRYPTION = 1;         //95#



#endif	/* DEFAULT_SETTINGS_H */
