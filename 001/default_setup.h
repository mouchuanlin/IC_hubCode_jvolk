/* 
 * File:   default_setup.h
 * Author: Scott
 *
 * Created on April 9, 2018, 3:15 PM
 */

#ifndef DEFAULT_SETUP_H
#define	DEFAULT_SETUP_H

#define FIRST_RUN_CHECK                     0xF0        // EEPROM address corresp. to this value

// EEPROM Addresses for each parameter
#define APN_ADDR                            0x10
#define IP1_ADDR                            0x30
#define IP2_ADDR                            0x50
#define IP3_ADDR                            0x70
#define IP4_ADDR                            0x90
#define PORT1_ADDR                          0xB0
#define PORT2_ADDR                          0xB2
#define PORT3_ADDR                          0xB4
#define PORT4_ADDR                          0xB6
#define SMS_WAIT_TIME_ADDR                  0xB8
#define ACCESS_CODE_ADDR                    0xC0
#define PROGRAM_ACK_ADDR                    0xC7
#define TESTING_FREQ_ADDR                   0xC8
#define SERVER_ACK_TIME_ADDR                0xC9
#define UNIT_ACCT_ADDR                      0xCA
#define LINE_CARD_ADDR                      0xD0
#define ZONE1_ADDR                          0xB9
#define ZONE2_ADDR                          0xBA
#define TP_PIN_ADDR                         0xBB
#define CYCLE_ADDR                          0xBC
#define RETRY_TIMES_ADDR                    0xBD
#define ENCRYPTION_ADDR                     0xE0

#define VER_ADDR0                           0x00
#define VER_ADDR1                           0x01
#define VER_ADDR2                           0x02


void write_defaults();

unsigned char const APN[]="#";              //35#
unsigned char const IP1[]="#";              //01#
unsigned char const IP2[]="#";              //02#
unsigned char const IP3[]="#";              //03#
unsigned char const IP4[]="#";              //04#
unsigned int const PORT1 = 2020;            //31#   
unsigned int const PORT2 = 2020;            //32#   
unsigned int const PORT3 = 2020;            //33#   
unsigned int const PORT4 = 2020;            //34#   
unsigned char const ACCESS_CODE[]="1111#";  //05#
unsigned char const PROGRAM_ACK=0x01;       //06#
unsigned char const TEST_FREQ=15;           //07#
unsigned char const SERVER_ACK_TIME=45;     //08#
unsigned char const UNIT_ACCT[]="0000#";   //10#
unsigned char const LINE_CARD[]="1#";       //11#
unsigned char const ZONE1=1;                //12#
unsigned char const ZONE2=20;               //13#
unsigned char const TP_PIN=0;               //14#
unsigned char const CYCLE = 2;              //15#
unsigned char const RETRY = 10;             //16#
unsigned char const SMS_WAIT_TIME = 10;     //09#
unsigned char const ENCRYPTION = 1;         //95#


   

#endif	/* DEFAULT_SETUP_H */

