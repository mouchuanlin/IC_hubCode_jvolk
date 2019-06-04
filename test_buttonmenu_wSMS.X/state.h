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
 * DEFINES
 ****************************************************/
#define SMS_LISTEN_TIMEOUT          3660        // 1min, given 1:16 pre/post scalers, 3.05 ticks = 100ms on tmr4
#define LEARN_SENSOR_TIMEOUT        1830
#define DEL_SENSOR_TIMEOUT          1830

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
    DEL_SENSOR,
    SEND_TEST
};
enum STATE state;
enum STATE prev_state;

uint16_t t4_tick = 0;



#endif	/* STATE_H */

