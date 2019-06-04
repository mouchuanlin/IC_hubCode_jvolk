/* 
 * File:   state.h
 * Author: Scott
 *
 * Created on May 16, 2018, 11:09 AM
 */

#ifndef STATE_H
#define	STATE_H

#include <stdint.h>


/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void check_state();
void control_leds();


/*****************************************************
 * VARIABLES
 ****************************************************/
enum STATE {
    OPERATIONAL,
    LISTEN_SMS,
    LEARN_SENSOR,
    DELETE_SENSOR
};
enum STATE state;



#endif	/* STATE_H */

