/*
 * File: data.c
 * Author: THE MASTER
 * 
 * This file defines all functions used in data transmission/reception from/to
 * the intended IP.
 * 
 * Date Created: 02 May 2018, 9:00 AM
 * Last Modified: 02 May 2018, 9:00 AM
 */


void send_data(uint8_t eventType)
{
    uint8_t numberRetries = ee_read(Retry_SMS);     // Number of times we retry.
                                                    // may require sending over
                                                    // another context, but this
                                                    // is NOT DONE YET.
    start_modem();
    while (!configure_socket_ok())
        start_modem();
    if (numberRetries > 0)
    {
        while (!transmit_over_socket_ok() && numberRetries-- > 0)
        {
            ledPattern = SEND_ERR;
            // Delay to get send_error LED pattern, then change led pattern to 
            // line fault
            wait_network();             // Wait for the specified time in EEPROM
                                        // before configuring and sending again
        }
        if (numberRetries != 0)
        {
            close_socket();
        }
    }
    else
        if (!transmit_over_socket_ok())
        {
            ledPattern = SEND_ERR;
            // Wait 5s, set ledPattern = line_fault, wait another 5s, then
            // come back and continue.
        }
    
}



bool transmit_over_socket_ok()
{
    /* returns true if ack successfully received OR no ack needed and everything
     else was transmitted with OK from UART */
}