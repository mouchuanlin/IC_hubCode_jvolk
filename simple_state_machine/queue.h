/* 
 * File:   queue.h
 * Author: Scott
 *
 * Created on April 30, 2018, 1:27 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void save_sensors();                // Save learned sensor IDs to EEPROM
void restore_sensors();             // Recall learned sensor IDs from EEPROM


/*****************************************************
 * VARIABLES
 ****************************************************/

// Transmit types:
uint8_t AlarmGB = 'A';
uint8_t AlarmLB = 'B';
uint8_t TamperGB = 'C';
uint8_t TamperLB = 'D';
uint8_t SupervisoryGB = 'E';
uint8_t SupervisoryLB = 'F';
uint8_t Test = 'G';

/* RF transmit types are OR'ed with 4 LSB that index the sensor within
 * the "learned sensor" array. In this way, the sensors can be distinguished
 * in a single 8-bit code. Note that if we need more sensors, the following
 * codes could be compressed to just 3 bits, but must be compared against 
 * transmit types listed above to avoid overlap. Also handle_queue() logic
 * will need alteration. rfTest event code will need to be accounted for.
 */
// Transmit types continued (RF, only 4 MSB):
uint8_t rfLearnCode = 0b11110000;
uint8_t rfSupervisoryGB = 0b10000000;
uint8_t rfSupervisoryLB = 0b10010000;
uint8_t rfAlarmGB = 0b11000000;
uint8_t rfAlarmLB = 0b11010000;
uint8_t rfTamperGB = 0b10100000;
uint8_t rfTamperLB = 0b10110000;
uint8_t rfTest = 0b01110000;


uint8_t learnedSensors[16];
uint8_t queue[20] = {0};    // up to 20 events allowed in queue at any
                            // time
uint8_t q_end_pointer = 0;  // points to final event in queue


#endif	/* QUEUE_H */

