/*
 * File: state.c
 * Author: THE MASTER
 * 
 * This file defines all functions used in control of the state machine;
 * delegates tasks associated with the 4 different states in the hub.
 * 
 * Date Created: 30 Apr. 2018, 10:19 AM
 * Last Modified: 30 Apr. 2018, 10:19 AM
 */

#include "config.h"
#include "state.h"
#include "queue.h"


void check_state()
{
    check_alarm_tamper();
    
    check_button();
    
    check_supervisory();
    
    switch (state)
    {
        case OPERATIONAL:
            ledPattern = WAIT;
            
            while (apn_ip_acct_are_default())
            {
                readyForSleep = false;
                state = LISTEN_SMS;
            }
            if (!apn_ip_acct_are_default() && !inButtonMenu && queue_is_empty() && buttonPressCnt == 0)
                readyForSleep = true;
            
            if (!apn_ip_acct_are_default() && !queue_is_empty())
                handle_queue();
            break;
            
        case ADD_SENSOR:
            ledPattern = SENSOR_ADD;
            add_sensor();
            state = OPERATIONAL;
            break;
            
        case DELETE_SENSOR:
            ledPattern = SENSOR_DELETE;
            delete_sensor();
            state = OPERATIONAL;
            break;
            
        case LISTEN_SMS:
            ledPattern = APN_IP_ACCT_NOT_SET;
            listen_sms();
            state = OPERATIONAL;
            break;
    }
}


void check_button()
{
    if (state == OPERATIONAL && inButtonMenu && !testButtonTimedOut)
        ledPattern = BUTTON_MENU;
    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut /
            && testButtonCnt == 1)
    {
        testButtonCnt = 0;
        testButtonTimedOut = false;
        add_to_queue(Test);             // after transmitting test, change state
                                        // to LISTEN_SMS
    }
    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut /
            && testButtonCnt == 2)
    {
        ledPattern = SENSOR_ADD;
        testButtonCnt = 0;
        testButtonTimedOut = false;
        state = ADD_SENSOR;
    }
    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut /
            && testButtonCnt == 3)
    {
        ledPattern = SENSOR_DELETE;
        testButtonCnt = 0;
        testButtonTimedOut = false;
        state = DELETE_SENSOR;
    }
    else if (state == OPERATIONAL && inButtonMenu && testButtonTimedOut /
            && testButtonCnt == 4)
    {
        ledPattern = IDLE;
        testButtonCnt = 0;
        testButtonTimedOut = false;
        inButtonMenu = false;
        state = OPERATIONAL;
    }
    else if (state == OPERATIONAL && !inButtonMenu && testButtonCnt == 0)
    {
        ledPattern = IDLE;
    }
    
    if (testButtonCnt == 5 && !inButtonMenu)
    {
        testButtonCnt = 0;
        inButtonMenu = true;
    }
}


void check_alarm_tamper()
{
    if ((alarm_int && !stillTriggered) || (tamper_int && !stillTampered))
    {
        __delay_ms(30);             // Debounce
        if (alarm_pin)
        {
            stillTriggered = true;
            if (isLowBattery())
                add_to_queue(AlarmLB);
            else
                add_to_queue(AlarmGB);
        }
        if (tamper_pin)
        {
            stillTampered = true;
            if (isLowBattery())
                add_to_queue(TamperLB);
            else
                add_to_queue(TamperGB);
        }
    }
    else if ((!alarm_int && stillTriggered) || (!tamper_int && stillTampered))
    {
        __delay_ms(20);
        if (!alarm_pin)
            stillTriggered = false;
        if (!tamper_pin)
            stillTampered = false;
    }
}


void check_supervisory()
{
    uint32_t testFreq = ee_read(TEST_FREQ);
    if (systemTick >= (_1HR * 24 * testFreq))
    {
        if (isLowBattery())
            add_to_queue(SupervisoryLB);
        else
            add_to_queue(SupervisoryGB);
        systemTick = 0;
    }
}




void add_sensor()           // Primary loop that controls add_sensor 
{
    if (id_buf_full())      // if device has already learned max number of sensors
        return;             // return immediately
    
    tmr0Tick = 0;           // Keep track of timer0 ticks with this variable
    start_timer0();         // control LEDs
    CLRWDT();
    while(tmr0Tick <= _2minutes)        // must increment tmr0Tick in tmr0 interrupt
    {
        if(receivedRF)
        {
            if(checksum_ok())          // store rf txmission in global rfBuf
            {
                if(!id_already_learned())
                    learn_id();
            }
        }
    }
    
}

void delete_sensor()        // Primary loop that controls delete sensor
{
    if (id_buf_empty())     // if device hasn't learned any sensors
        return;             // return immediately
    
    uint8_t id_index = 0;
    tmr0Tick = 0;           // Keep track of timer0 ticks with this variable
    start_timer0();         // control LEDs
    CLRWDT();
    while(tmr0Tick <= _2minutes)        // must increment tmr0Tick in tmr0 interrupt
    {
        if(receivedRF)
        {
            if (checksum_ok())          // store rf txmission in global rfBuf
            {
                if (id_already_learned(*id_index))  // put index of matching id
                                                    // in variable id_index
                {
                    remove_id(id_index);
                }
                if (id_buf_empty())     // if device hasn't learned any sensors
                    return;             // return immediately
            }
        }
    }
}



void control_leds()
{
    g_tmr0_tick++;
    b_tmr0_tick++;
    switch (ledPattern)
    {
        case IDLE:
            G_LED = OFF;
            B_LED = OFF;
            g_tmr0_tick = 0;
            b_tmr0_tick = 0;
            break;
            
        case WAIT:
            B_LED = OFF;
            b_tmr0_tick = 0;
            switch (g_tmr0_tick)
            {
                case 1:
                    G_LED = ON;
                    break;
                case 6:
                    G_LED = OFF;
                    break;
                case 15:
                    g_tmr0_tick = 0;
                    break;
            }
            break;
            
        case STANDBY:               // green LED turns ON_0.5s / OFF_1.5s
            G_LED = OFF;
            g_tmr0_tick = 0;
            switch (b_tmr0_tick)
            {
                case 1:
                    B_LED = ON;
                    break;
                case 6:
                    B_LED = OFF;
                    break;
                case 15:
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENDING:               // blue LED turns ON_0.5s / OFF_1.5s
            G_LED = OFF;
            g_tmr0_tick = 0;
            switch (b_tmr0_tick)
            {
                case 1:
                    B_LED = ON;
                    break;
                case 6:
                    B_LED = OFF;
                    break;
                case 15:
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case BUTTON_MENU:
            G_LED = ON;
            B_LED = ON;
            g_tmr0_tick = 0;
            b_tmr0_tick = 0;
            break;
            
        case APN_IP_ACCT_NOT_SET:       // Both LEDs blink oppositely (0.5s)
            switch(g_tmr0_tick)
            {
                case 1:
                    G_LED = ON;
                    B_LED = OFF;
                    break;
                case 6:
                    G_LED = OFF;
                    B_LED = ON;
                    break;
                case 10:
                    g_tmr0_tick = 0;
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SEND_ERR:                  // Blue-green-green cross-flash (0.1s)
            switch(g_tmr0_tick)
            {
                case 1:
                    G_LED = OFF;
                    B_LED = ON;
                    break;
                case 2:
                    G_LED = OFF;
                    B_LED = OFF;
                    break;
                case 3:
                    G_LED = ON;
                    B_LED = OFF;
                    break;
                case 4:
                    G_LED = OFF;
                    B_LED = OFF;
                    break;
                case 5:
                    G_LED = ON;
                    B_LED = OFF;
                    break;
                case 6:
                    G_LED = OFF;
                    B_LED = OFF;
                    g_tmr0_tick = 0;
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENSOR_ADD:
            G_LED = ON;
            g_tmr0_tick = 0;
            switch (b_tmr0_tick)
            {
                case 1:
                    B_LED = ON;
                    break;
                case 6:
                    B_LED = OFF;
                    break;
                case 20:
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case SENSOR_DELETE:
            B_LED = ON;
            b_tmr0_tick = 0;
            switch (g_tmr0_tick)
            {
                case 1:
                    G_LED = ON;
                    break;
                case 6:
                    G_LED = OFF;
                    break;
                case 20:
                    g_tmr0_tick = 0;
                    break;
            }
            break;
            
        case LINE_FAULT:            // Both LEDs ON_0.3s / OFF_4.7s
            switch (g_tmr0_tick)
            {
                case 1:
                    G_LED = ON;
                    B_LED = ON;
                    break;
                case 4:
                    G_LED = OFF;
                    B_LED = OFF;
                    break;
                case 47:
                    g_tmr0_tick = 0;
                    b_tmr0_tick = 0;
                    break;
            }
            break;
            
        case RF_INT:                // Hold here for as long as you want within
                                    // state machine
            G_LED = ON;
            B_LED = ON;
            g_tmr0_tick = 0;
            b_tmr0_tick = 0;
            break;
    }
            
            
}