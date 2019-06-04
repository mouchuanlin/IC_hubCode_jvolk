/* 
 * File:   module.h
 * Author: Scott
 *
 * Created on April 6, 2018, 1:59 PM
 */

#ifndef MODULE_H
#define	MODULE_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"
#include "default_setup.h"


/*****************************************************
 * DEFINES
 ****************************************************/
#define LE866_SV1                   // Write module name here


/*****************************************************
 * VARIABLES RELATING TO MODULE CONTEXT
 ****************************************************/
#ifdef LE866_SV1
    const uint8_t scfg[] = "AT#SCFG=1,3,300,90,600,50\r\n";
    const uint8_t cgdcont[] = "AT+CGDCONT=3,\"IP\",\"";
    const uint8_t sgact[] = "AT#SGACT=3,1\r\n";
#endif
#ifdef LE910_NA1
    const uint8_t scfg[] = "AT#SCFG=1,1,300,90,600,50\r\n";
    const uint8_t cgdcont[] = "AT+CGDCONT=1,\"IP\",\"";
    const uint8_t sgact[] = "AT#SGACT=1,1\r\n";
#endif

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void md_restart();
bool wakeup_module_ok();
bool startup_md_ok();
bool md_io_setup_ok();
void delay5ms(unsigned int cnt);
bool is_first_run();
void save_first_run();
bool init_modem_ok();
bool sim_card_ok();
bool registration_ok();
bool md_attached();
bool socket_configured();
bool set_pdp_ok();
bool open_pdp_ok();
bool open_connection();
bool is_connected();

#endif	/* MODULE_H */

