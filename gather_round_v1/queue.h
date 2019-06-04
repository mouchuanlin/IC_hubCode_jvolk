/* 
 * File:   queue.h
 * Author: Scott
 *
 * Created on September 14, 2018, 8:26 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H

// INCLUDES
#include <stdbool.h>
#include <stdint.h>

// DEFINES
#define MAX_EVENT       20



enum EVENT {
    SMOKE_ALARM,    // 111
    TEST_CYCLE,     // 602
    TAMPER_OPEN,    // 383
//    TAMPER_CLOSE,   //
    TEST_PIN,       // 601
    LOW_BATTERY,    // 384
    EOL,            // 
    FLOOD,          // 154
    GLASS,          // 140
    SUPERVISORY,    // 147
    CARBON,         // 162
    MOTION,         // 132
    DOOR,           // 134
    PANIC,          // 101
    HVAC,           // 161
    APPLIANCE,      // 152
    RESERVE1,       // 155
    RESERVE2,       // 180
    RESERVE3,       // 181
    RESERVE4,       // 182
    RESERVE5,       // 183
    RESERVE6,       // 184
    RESERVE7        // 185
};

// FUNCTION PROTOS
enum EVENT get_next_event(uint8_t *zone);
bool event_queue_is_empty();
uint8_t add_event(enum EVENT event, uint8_t zone);
void remove_event(uint8_t index);

// VARIABLES
//                 [EventType][sensor #]
uint8_t event_queue[MAX_EVENT][2];
uint8_t queue_end_ptr = 0;

#endif	/* QUEUE_H */

