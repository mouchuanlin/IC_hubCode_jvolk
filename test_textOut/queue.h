/* 
 * File:   queue.h
 * Author: Scott
 *
 * Created on April 6, 2018, 8:30 AM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#include "config.h"

/*****************************************************
 * PROTOTYPES
 ****************************************************/
void push(uint8_t byte_to_push);
uint8_t pop();
void clear_queue();


/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t event_queue[30];          // Queue can store up to 30 operations at once
uint8_t q_index;            // Indexes the queue

#endif	/* QUEUE_H */