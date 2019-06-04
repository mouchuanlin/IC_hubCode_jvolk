/*
 * File: queue.c
 * Author: THE MASTER
 * 
 * This file defines all functions related to queue handling.
 * 
 * Date Created: 30 Apr. 2018, 13:02 PM
 * Last Modified: 30 Apr. 2018, 13:02 PM
 */

#include "config.h"
#include "queue.h"

void init_queue()
{
    clear_queue();
}

void handle_queue()
{
    if (!md_started)
        start_modem();
    uint8_t index = 0;
    uint8_t nextEvent = get_next_from_queue(*index);
    if (nextEvent == AlarmGB || nextEvent == AlarmLB)
    {
        send_data(nextEvent);
        remove_from_queue(index);
        state = OPERATIONAL;
    }
    else if (nextEvent == TamperGB || nextEvent == TamperLB /
            || nextEvent & 0x80 != 0)                           // RF indicated by MSB set
    {
        send_data(nextEvent);
        remove_from_queue(index);
        state = OPERATIONAL;
    }
    else if (nextEvent == SupervisoryGB || nextEvent == SupervisoryLB /
            || nextEvent == Test || nextEvent & 0xF0 == rfTest)
    {
        send_data(nextEvent);
        remove_from_queue(index);
        state = LISTEN_SMS;         // Go into listen mode after sending test or super
    }
}

void add_to_queue(uint8_t cmd)
{
    queue[q_end_pointer++] = cmd;
}

void remove_from_queue(uint8_t index)       // Removes single event from queue
{
    queue[index] = 0;                       // clear that position in queue
    for (uint8_t i = index; i < q_end_pointer-1; i ++)
        queue[i] = queue[i++];                          // Shift all later events by
                                                        // one index backwards
    q_end_pointer--;
}

void clear_queue()
{
    q_end_pointer = 0;
    for (uint8_t i = 0; i < sizeof(queue); i ++)
        queue[i] = 0;                           // fill queue with zeroes
}

bool queue_is_empty()
{
    return (q_end_pointer <= 0);
}

uint8_t get_next_from_queue(uint8_t *index)
{
    uint8_t event;
    /* Parse queue for alarm event first; i.e. top priority */
    for (uint8_t i = 0; i < q_end_pointer; i ++)
    {
        event = queue[i];
        if (event == AlarmGB || event == AlarmLB)
        {
            index = i;
            return event;
        }
    }
    /* Parse queue for tamper, supervisory, and RF event second; i.e. mid-priority */
    for (uint8_t i = 0; i < q_end_pointer; i ++)
    {
        event = queue[i];
        if (event == TamperGB || event == TamperLB /
                || event == SupervisoryGB || event == SupervisoryLB /
                || event & 0x80 != 0)
        {
            index = i;
            return event;
        }
    }
    /* Lastly, parse queue for test; i.e. low priority */
    for (uint8_t i = 0; i < q_end_pointer; i ++)
    {
        event = queue[i];
        if (event == Test)
        {
            index = i;
            return event;
        }
    }
}