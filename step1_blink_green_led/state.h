/* 
 * File:   state.h
 * Author: Scott
 *
 * Created on October 16, 2018, 5:10 PM
 */

#ifndef STATE_H
#define	STATE_H

#include <stdint.h>

/*****************************************************
 * DEFINES
 ****************************************************/

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void control_leds();


/*****************************************************
 * VARIABLES
 ****************************************************/
enum HUB_STATE {
    NREM,
    IN_BUTTON_MENU,
    LISTEN_SMS,
    LEARN_SENSOR,
    DEL_SENSOR,
    SEND_TEST
};
enum HUB_STATE STATE;
enum HUB_STATE PREV_STATE;


#endif	/* STATE_H */

