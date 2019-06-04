/* 
 * File:   modem.h
 * Author: Scott
 *
 * Created on October 17, 2018, 2:49 PM
 * 
 */

#ifndef MODEM_H
#define	MODEM_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>


/*****************************************************
 * DEFINES
 ****************************************************/



/*****************************************************
 * FUNCTION PROTOS
 ****************************************************/
void start_modem();


/*****************************************************
 * VARIABLES
 ****************************************************/
bool modemAwake = false;
bool modemMayBeAwake = false;
bool waitingForModemBoot = false;


#endif	/* MODEM_H */

