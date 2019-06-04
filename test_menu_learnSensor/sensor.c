/*
 * File: sensor.c
 * Author: THE MASTER
 * 
 * Date Created: 16 May 2018, 03:59 PM
 */

#include "config.h"
#include "sensor.h"

bool alreadyLearned(uint8_t *id, uint8_t numIdBytes)
{
    /* Note that with this version, number of bytes in the ID is expectedly
     * 3, including device type.
     */
    uint8_t pointer = *id;
    for (uint8_t i = 0; i < learnedSensorPtr; i++)
    {
        if (pointer == learnedSensors[i][0])
        {
            pointer++;
            if (pointer == learnedSensors[i][1])
            {
                pointer++;
                if (pointer == learnedSensors[i][2])
                    return true;
                else
                    pointer -= 2;
            }
            else
                pointer --;
        }
    }
    return false;
}


void add_sensor(uint8_t *id, uint8_t numIdBytes)
{
    if (learnedSensorPtr >= MAX_SENSORS)    // Don't try to add sensor if we're
        return;                             // at maximum allowed number.
    for(uint8_t i = 0; i < numIdBytes; i++)
    {
        learnedSensors[learnedSensorPtr][i] = id++;
    }
    learnedSensorPtr++;
}