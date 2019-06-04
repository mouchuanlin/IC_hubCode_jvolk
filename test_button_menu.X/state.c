/*
 * File: state.c
 * Author: THE MASTER
 * 
 * Date Created: 16 May 2018, 10:51 AM
 */
#include "config.h"
#include "state.h"

bool buttonStillPressed = false;
bool waiting = true;

void check_state()
{
    switch (state)
    {
        case OPERATIONAL:
            waiting = false;
            // Test pin operation only enabled when in main operation loop
            if (TEST_PIN && !buttonStillPressed)           // Check button
            {
                __delay_ms(30);         // De-bounce
                if (TEST_PIN)
                {
                    buttonPressCount++;
                    reload_tmr3_pt25s();
                    tmr3_cnt = 0;
                }
                buttonStillPressed = true;
            }
            else                                // Reset button
            {
                __delay_ms(30);
                if (!TEST_PIN)
                {
                    buttonStillPressed = false;
                }
            }
            if (inButtonMenu && buttonPressCount == 0)
            {
                G_ON();
                B_ON();
            }
            else
            {
                G_OFF();
                B_OFF();
            }
            bled_tmr0_tick = 0;
            gled_tmr0_tick = 0;
            break;
            
        case LISTEN_SMS:
            break;
            
        case LEARN_SENSOR:
            break;
            
        case DELETE_SENSOR:
            break;
    }
}


void control_leds()
{
    switch (state)
    {
        case OPERATIONAL:
            if (waiting)
            {
                switch(gled_tmr0_tick)
                {
                    case 1:
                        G_ON();
                        break;
                    case 5:
                        G_OFF();
                        break;
                    case 20:
                        gled_tmr0_tick = 0;
                        break;
                }
                B_OFF();
                bled_tmr0_tick = 0;
            }
            break;
            
        case LISTEN_SMS:
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_ON();
                    break;
                case 2:
                    B_OFF();
                    bled_tmr0_tick = 0;
                    break;
            }
            G_OFF();
            gled_tmr0_tick = 0;
            break;
            
        case LEARN_SENSOR:
            switch (bled_tmr0_tick)
            {
                case 1:
                    B_ON();
                    break;
                case 5:
                    B_OFF();
                    break;
                case 20:
                    bled_tmr0_tick = 0;
                    break;
            }
            G_ON();
            gled_tmr0_tick = 0;
            break;
            
        case DELETE_SENSOR:
            switch (gled_tmr0_tick)
            {
                case 1:
                    G_ON();
                    break;
                case 5:
                    G_OFF();
                    break;
                case 20:
                    gled_tmr0_tick = 0;
                    break;
            }
            B_ON();
            bled_tmr0_tick = 0;
            break;
    }
}