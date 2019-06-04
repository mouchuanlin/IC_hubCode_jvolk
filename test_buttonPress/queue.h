/* 
 * File:   queue.h
 * Author: THE MASTER
 *
 * Created on April 26, 2018, 11:40 AM
 */

#ifndef QUEUE_H
#define	QUEUE_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"


/*****************************************************
 * DEFINES
 ****************************************************/
#define MAX_COMMANDS        20      // Number of maximum number of commands allowed in queue
                                    // at any given time.

/*****************************************************
 * STRUCTS
 ****************************************************/

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void init_queue();
void q_add_cmd(uint8_t cmd);
void q_clear();
bool q_is_empty();
uint8_t q_read(uint8_t index);
uint8_t q_get_next_index();
bool q_contains(uint8_t cmd);


/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t q_end = 0;              // Note that q_index should not be touched
uint8_t q_index = 0;            // by the main program; it should only be used 
                                // in queue.c. q_end should point to the end of the
                                // command list
uint8_t queue[MAX_COMMANDS];

const uint8_t q_alarm = 'A';
const uint8_t q_tamper = 'T';
const uint8_t q_manualtest = 'M';
const uint8_t q_lowbatt = 'B';
const uint8_t rf_cmd_mask = 0xF0;       // Mask command from RF transmission
const uint8_t rf_id_mask = 0x0F;        // Mask serial ID index from RF transmission


#endif	/* QUEUE_H */

