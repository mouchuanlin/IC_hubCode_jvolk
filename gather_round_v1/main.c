/* 
 * File:   main.c
 * Author: JV
 *
 * Created on September 14, 2018, 8:20 PM
 */

// INCLUDES
#include "config.h"
#include "modem.h"
#include "queue.h"
#include "uart.h"



void main() 
{
    init_pic();

    load_ID();          // load ID into volatile memory for quicker access
    while(1)
    {
        CLRWDT();
        check_state();
        
        if (event_queue_is_empty() && STATE == IDLE && test_count == 0 && MD_STATE == MD_OFF)
        {
            disable_md_rxint();
            terminate_md_uart();
            terminate_rf_uart();
            SLEEP();
            NOP();
        }
        else if (!event_queue_is_empty())
        {
//            send_data(get_next_event());
        }
        SLEEP();
        NOP();
    }
}



void check_state()
{
    switch(STATE)
    {
        case IDLE:
            check_super();
            check_tamper();         // ERR pin - low batt & tamper & EOL & malfunction signalling 
                                    // from motherboard, EOL and malfunction not yet integrated
            check_alarm();          // ALARM pin
            check_test();           // TEST pin
//            check_sensor();
//            check_event();
            break;
        case LISTEN_SMS:
            CLRWDT();
            listen_sms();
            check_tamper();         // ERR pin - low batt & tamper & EOL & malfunction signalling 
                                    // from motherboard, EOL and malfunction not yet integrated
            check_alarm();          // ALARM pin
            check_test();           // TEST pin
//            check_sensor();
            break;
        case LEARN_SENSOR:
            CLRWDT();
            IOCBbits.IOCB5 = 1;
            INTCONbits.RBIE = 1;
            if (receivedSensor && idEndPtr < (sizeof(ID) - 1))
            {
                add_sensor(rx2_buf);
                receivedSensor = false;
            }
            if (t4_tick >= SENSOR_MODE_TIMEOUT)
            {
                stop_sensor_tmr();
                STATE = PREV_STATE;
            }
                break;
        case DEL_SENSOR:
            CLRWDT();
            del_sensor(rx2_buf);
            if (t4_tick >= SENSOR_MODE_TIMEOUT)
            {
                stop_sensor_tmr();
                STATE = PREV_STATE;
            }
            break;
    }
}
    
void __interrupt isr()
{
    uint8_t temp;
    if (INTCONbits.RBIF)
    {
        INTCONbits.RBIF = 0;
        init_uart();
    }
    if (PIR3bits.RC2IF && PIE3bits.RC2IE)
    {
        PIR3bits.RC2IF = 0;
        temp = RC2REG;
        rx2_buf[rx2pos++] = temp;
        if (rx2pos >= sizeof(rx2_buf))  rx2pos--;
        if (rx2pos >= sizeof(rx2_buf) && rx2_buf[4] == '\n')
            receivedSensor = true;
    }
    if (RC1IF && PIE1bits.RCIE)
    {
        RC1IF = 0;
        currentChar = RC1REG;
        mdRxBuf[endPtr++] = currentChar;
        if (endPtr >= sizeof(mdRxBuf))
            endPtr--;
        if (endPtr > 2)
        {
            // Check for OK or ERROR
            if (mdRxBuf[(uint8_t)(endPtr - 2)] == 'O' && mdRxBuf[(uint8_t)(endPtr - 1)] == 'K' \
                    && (mdRxBuf[endPtr] == '\r' || mdRxBuf[endPtr] == '\n'))
            {
                tmr5_cnt = 0;
                disable_tmr5();             // Turn off timer & disable associated interrupt
                TMR5IF = 0;
                receivedOK = true;
                receivedERR = false;
                timedOut = false;
                disable_md_rxint();
            }
            else if (endPtr >= 5 && mdRxBuf[(uint8_t)(endPtr - 4)] == 'E' && \
                     mdRxBuf[(uint8_t)(endPtr - 3)] == 'R' && mdRxBuf[(uint8_t)(endPtr - 2)] == 'R' && \
                     mdRxBuf[(uint8_t)(endPtr - 1)] == 'O' && mdRxBuf[endPtr] == 'R')
            {
                tmr5_cnt = 0;
                disable_tmr5();             // Turn off timer & disable associated interrupt
                TMR5IF = 0;
                receivedOK = false;
                receivedERR = true;
                timedOut = false;
                disable_md_rxint();
            }
        }
    }
    // UART time-out and modem boot timer
    if (TMR5IF)
    {
        // timer 5 interrupts every 250ms
        tmr5_cnt++;
        TMR5IF = 0;
        reload_tmr5_pt25s();
        // 2s timer; 
//        check_mdstate_tmr5();
    }
    
    if (TMR0IF)
    {
        TMR0IF = 0;
        reload_timer0();
        gled_tmr0_tick++;
        bled_tmr0_tick++;
        control_leds();
    }
    if (TMR3IF)
    {
        tmr3_cnt++;
        TMR3IF = 0;
        // 2s timer; 
        if (tmr3_cnt >= 8 && ((inButtonMenu && buttonPressCount > 0) || !inButtonMenu))
        {
            tmr3_cnt = 0;
            if (inButtonMenu && buttonPressCount == 1)
            {
                inButtonMenu = false;       // Leave button menu if we're in it
                disable_tmr3();
                PREV_STATE = STATE;
                STATE = LISTEN_SMS;
            }
            else if (inButtonMenu && buttonPressCount == 2)
            {
                inButtonMenu = false;       // Leave button menu if we're in it
                disable_tmr3();
                PREV_STATE = STATE;
                STATE = LEARN_SENSOR;
                start_sensor_tmr();
            }
            else if (inButtonMenu && buttonPressCount == 3)
            {
                inButtonMenu = false;       // Leave button menu if we're in it
                disable_tmr3();
                PREV_STATE = STATE;
                STATE = DEL_SENSOR;
                start_sensor_tmr();
            }
            else if (inButtonMenu && buttonPressCount == 4)
            {
                inButtonMenu = false;       // Leave button menu if we're in it
                disable_tmr3();
                add_event(TEST_PIN, (uint8_t)(ee_read(0x00, HUB_ZONE_ADDR)));             // If test is in queue, goes into listen mode after
            }                                   // sending text.
            else if (buttonPressCount == 5)
            {
                inButtonMenu = (bool)(~inButtonMenu);       // toggle menu on/off
                tmr3_cnt = 0;
                STATE = IDLE;
            }   
            
            buttonPressCount = 0;       // clear button presses once we extract next state info
        }
        else if (tmr3_cnt >= 40 && inButtonMenu && buttonPressCount == 0) // 10s timeout
        {
            inButtonMenu = false;
            tmr3_cnt = 0;
            disable_tmr3();
        }
    }
}


