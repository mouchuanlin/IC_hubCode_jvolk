/* 
 * File:   queue.c
 * Author: THE MASTER
 *
 * Created on April 26, 2018, 11:40 AM
 */

#include "config.h"
#include "queue.h"

/*
 * 
 */

void q_add_cmd(uint8_t cmd)
{
    queue[++q_index];
}

void q_clear()
{
    
}


bool q_is_empty()
{
    return true;
}


uint8_t q_read(uint8_t index)
{
    uint8_t i = 0;
    return i;
}

/* The following function returns not q_point but the index of the next command
 * with the highest priority. Priority is assigned as follows (highest being #1):
 * 
 * 1. Internal alarm (smoke detection from motherboard)
 * 2. Internal tamper
 *  Any non-supervisory message from receiver (alarm, tamper, low batt on a FIFO
 *      basis)
 *  Internal low battery
 *  Internal supervisory
 *  EOL (YET TO BE IMPLEMENTED FOR ANY DEVICE IN THIS PROTOCOL)
 *  Any supervisory message received over RF
 * 3. Listen Mode/test
 */
uint8_t q_get_next_index()
{
//    if (queue_cmd.contains(q_alarm)){}
    return 0;
}

bool q_contains(uint8_t cmd)
{
    return true;
}


void init_queue()
{
    q_clear();
}