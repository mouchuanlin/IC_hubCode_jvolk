
#include "config.h"
#include "queue.h"


enum EVENT get_next_event(uint8_t *zone)
{
    // NO prioritization yet
    zone = &event_queue[0][1];
    return event_queue[0][0];      // FIFO
}

bool event_queue_is_empty()
{                                           // end ptr keeps track of n+1'th element
    return (bool)(queue_end_ptr > 0);       // where n is the last element
}


uint8_t add_event(enum EVENT event, uint8_t zone)         // checks whether sensor is learned 
{
    if (queue_end_ptr == 0)
    {
        event_queue[0][0] = event;
        queue_end_ptr++;
    }
    else
    {
        for (uint8_t i = 0; i < queue_end_ptr; i++)
        {
            if (event_queue[i][0] == event && event_queue[i][1] == zone)
                return 0xFF;            // don't add
        }
        // If it's reached the end of the queue without finding duplicate,
        // add it.
        event_queue[queue_end_ptr][0] = event;
        event_queue[queue_end_ptr][1] = zone;
        if (queue_end_ptr++ >= sizeof(event_queue))
            queue_end_ptr--;                        // if queue is full, write over the last entry
    }
    return 0x01;
}


void remove_event(uint8_t index)
{
    if (index <= queue_end_ptr)     // if it's a valid index
    {
        event_queue[index][0] = 0x00;// clear event
        event_queue[index][1] = 0x00;// clear zone
        for (uint8_t i = index; i < queue_end_ptr; i++)
        {
            event_queue[i][0] = event_queue[(uint8_t)(i + 1)][0];
            event_queue[i][1] = event_queue[(uint8_t)(i + 1)][1];
        }
    }
}
