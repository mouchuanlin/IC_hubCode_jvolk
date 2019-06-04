/*
 * File: state.c
 * Author: THE MASTER
 * 
 * This file controls the state machine and associated LEDs. Interrupt-
 * driven in main.
 * 
 * 
 * Created on April 25, 2018, 15:10
 */

#include "config.h"
#include "queue.h"


void control_md_state()
{
    gled_tmr0_tick++;
    bled_tmr0_tick++;
    switch (MODEM_STATE)
    {
        /* IDLE Q EMPTY state: One of two idle modes. Queue is decided empty
         * and pic can go to sleep if so.
         */
        case IDLE_Q_EMPTY:          // Both LEDs OFF
            if (q_is_empty() && !inButtonMenu)
            {
                G_LED = 1;
                B_LED = 1;
                gled_tmr0_tick = 0;
                bled_tmr0_tick = 0;
                ready_for_sleep = true;
            }
            else if (!q_is_empty() && !inButtonMenu)
            {
                MODEM_STATE = IDLE_Q_NOT_EMPTY;
                gled_tmr0_tick = 0;
                bled_tmr0_tick = 0;
                ready_for_sleep = false;
            }
            else if (inButtonMenu)                // inButtonMenu = true
            {
                MODEM_STATE = BUTTON_PUSH_MENU;
                gled_tmr0_tick = 0;
                bled_tmr0_tick = 0;
                ready_for_sleep = false;
            }
            break;
            
            
        /* IDLE Q NOT EMPTY state: Second of two idle modes.
         * Queue is already decided NOT EMPTY. Haven't woken modem yet.
         */
        case IDLE_Q_NOT_EMPTY:      // green LED ON_0.5s / OFF_1.5s
            ready_for_sleep = false;
            switch(gled_tmr0_tick)
            {
                case 1:
                    G_LED = 0;
                    break;
                case 6:
                    G_LED = 1;
                    break;
                case 15:
                    gled_tmr0_tick = 0;
                    break;
            }
            B_LED = 1;              // blue LED OFF
            bled_tmr0_tick = 0;
            break;
            
            
        /* WAITING FOR MODEM state: Queue already determined NOT EMPTY.
         * Waiting for modem to wake up or to respond. May be configuring modem
         * in this state.
         */
        case WAITING_FOR_MODEM:          // green LED ON_0.5s / OFF_1.5s
            switch(gled_tmr0_tick)
            {
                case 1:
                    G_LED = 0;
                    break;
                case 6:
                    G_LED = 1;
                    break;
                case 15:
                    gled_tmr0_tick = 0;
                    break;
            }
            B_LED = 1;              // blue LED OFF
            bled_tmr0_tick = 0;
            break;
            
        /* BUTTON PUSH MENU state: After any button pushes, unit will enter this
         * "menu" mode. Once 5 button presses are counted, 
         * both LEDs illuminate for user. Next button
         * press sequence will determine which option the user wants.
         */
            // KEEP TRACK OF BUTTON PRESSES HERE
        case BUTTON_PUSH_MENU:
            if (enableTransmitLEDs)
            {
                inButtonMenu = false;
                MODEM_STATE = MD_TRANSMIT;
            }
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_LED = 0;
                    B_LED = 0;
                    gled_tmr0_tick = 0;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
            
        /* CONNECTING TO NETWORK state: configuring connection to server, before
         * CGDCONT, SGACT, SSENDEXT commands are issued for PDP activation and
         * data transmit through socket.
         */
        case CONNECTING_TO_NETWORK:      // green LED ON_0.5s / OFF_1.5s
            switch(gled_tmr0_tick)
            {
                case 1:
                    G_LED = 0;
                    break;
                case 6:
                    G_LED = 1;
                    break;
                case 15:
                    gled_tmr0_tick = 0;
                    break;
            }
            B_LED = 1;              // blue LED OFF
            bled_tmr0_tick = 0;
            break;
            
        /* SMS LISTEN state: state in which user can text the unit via SMS to
         * re-program the critical variable settings such as APN, IP, Account
         * Number, retry time, etc.
         */
        case SMS_LISTEN:
            G_LED = 1;              // green LED OFF
            gled_tmr0_tick = 0;
            switch(bled_tmr0_tick)
            {
                case 1:
                    B_LED = 0;
                    break;
                case 6:             // blue LED turns ON_0.5s / OFF_1.5s
                    B_LED = 1;
                    break;
                case 15:
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
            
        /* SMS PROCESSING state: SMS command is received; modem parsing the text
         * to:
         *  1. verify the command was valid (through use of the access code)
         *  2. set up environment to program the parameter into appropriate 
         *  subfield in EEPROM memory.
         */
        case SMS_PROCESSING:
            G_LED = 1;              // green LED OFF
            gled_tmr0_tick = 0;
            switch(bled_tmr0_tick)
            {
                case 1:
                    B_LED = 0;
                    break;
                case 6:             // blue LED turns ON_0.5s / OFF_1.5s
                    B_LED = 1;
                    break;
                case 15:
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
            
        /* SENSOR LEARN state: modem waiting for RF communication. Only one
         * sensor can be learned in at a time, as this state WILL GO TO RF_RECEIVED
         * STATE when communication was received. If we're in this mode and receive
         * transmission, check if ID already exists in buffer of learnedSensors[].
         * If not, add it.
         * May need to go between this mode and RF_RECEIVED. In this case you'll need to 
         * add memory to keep track: previousState = SENSOR_LEARN, nextState = RF
         */
        case SENSOR_LEARN:
            G_LED = 0;                  // green LED ON
            gled_tmr0_tick = 0;
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_LED = 0;          // blue LED turns ON_0.5s / OFF_2s
                    break;
                case 6:
                    B_LED = 1;
                    break;
                case 20:
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENSOR_DELETE:
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_LED = 0;          // green LED turns ON_0.5s / OFF_2s
                    break;
                case 6:
                    G_LED = 1;
                    break;
                case 20:
                    gled_tmr0_tick = 0;
                    break;
            }
            B_LED = 0;                  // blue LED ON
            bled_tmr0_tick = 0;
            break;
            
        case MD_TRANSMIT:
            G_LED = 1;                  // green LED OFF
            gled_tmr0_tick = 0;
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_LED = 0;          // blue LED turns ON_0.1s / OFF_0.1s
                    break;
                case 2:
                    B_LED = 1;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case NETWORK_ERR:
            switch (gled_tmr0_tick)
            {
                case 1:                     // Both LEDs ON_0.3s / OFF_4.7s
                    G_LED = 0;
                    B_LED = 0;
                    break;
                case 4:
                    G_LED = 1;
                    B_LED = 1;
                    break;
                case 47:
                    gled_tmr0_tick = 0;
                    bled_tmr0_tick = 0;
                    break;
            }
            break;
            
        case RF_INTERRUPT:
            switch (gled_tmr0_tick)
            {
                case 1:                     // green LED ON_2s / OFF and then go
                                            // to IDLE_Q_NOT_EMPTY and WAKING MODEM state
                    G_LED = 0;
                    break;
                case 21:
                    G_LED = 1;
                    gled_tmr0_tick = 0;
                    MODEM_STATE = WAITING_FOR_MODEM;
                    break;
            }
            B_LED = 1;
            bled_tmr0_tick = 0;
            break;
    }
    
    if (MODEM_STATE == NETWORK_ERR)
    {
        if (bled_tmr0_tick > 47 || gled_tmr0_tick > 47)
        {
            gled_tmr0_tick = 0;
            bled_tmr0_tick = 0;
        }
    }
    else if (MODEM_STATE == RF_INTERRUPT)
    {
        if (bled_tmr0_tick >= 21 || gled_tmr0_tick >= 21)
        {
            gled_tmr0_tick = 0;
            bled_tmr0_tick = 0;
        }
    }
    else if (bled_tmr0_tick > 20 || gled_tmr0_tick > 20)
    {
        gled_tmr0_tick = 0;
        bled_tmr0_tick = 0;
    }
}