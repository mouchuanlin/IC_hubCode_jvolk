/* 
 * File:   sensor.h
 * Author: THE MASTER
 *
 * Created on May 16, 2018, 3:40 PM
 */

#ifndef SENSOR_H
#define	SENSOR_H


/*****************************************************
 * INCLUDES
 ****************************************************/
#include <stdint.h>
#include <stdbool.h>


/*****************************************************
 * DEFINES
 ****************************************************/
#define MAX_SENSORS             16
#define MAX_ID_BYTES            3


/*****************************************************
 * FUNCTION PROTOS
 ****************************************************/
bool alreadyLearned(uint8_t *id, uint8_t numIdBytes);
void add_sensor(uint8_t *id, uint8_t numIdBytes);
void save_sensors();        // Save all learned sensors to EEPROM
void restore_sensors();     // Restore all learned sensors from EEPROM
//void broadcast_sensors();   // 1111#20#SENSOR#

/*****************************************************
 * VARIABLES
 ****************************************************/
uint8_t learnedSensors[MAX_SENSORS][MAX_ID_BYTES];
uint8_t learnedSensorPtr;       // Points to end of learned sensor buf


#endif	/* SENSOR_H */

