/* 
 * File:   queue.c
 * Author: Jennifer
 * 
 * This file contains all functions related to queue control.
 *
 * Created on April 6, 2018, 8:28 AM
 */

#include "config.h"
#include "queue.h"

void push(uint8_t byte_to_push)
{
    event_queue[++q_index] = byte_to_push;
}

uint8_t pop()
{
    return event_queue[q_index--];
}

void clear_queue()
{
    for (uint8_t i = 0; i < sizeof(event_queue); i ++)
        event_queue[i] = 0;
    q_index = 0;
}