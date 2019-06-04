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
#include "default_settings.h"
#include "uart.h"


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void start_modem();
bool startup_modem_ok();
void restart_modem();
void delay5ms(unsigned int cnt);
bool wakeup_modem_ok();
bool setup_modem_io_ok();
bool init_modem_io_ok();
bool sim_card_ok();

#endif	/* MODEM_H */

