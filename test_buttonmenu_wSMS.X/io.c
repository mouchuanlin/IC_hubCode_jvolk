

#include "config.h"
#include "modem.h"
#include "state.h"

void check_state_timeout()
{
    switch (state)
    {
        case LISTEN_SMS:
            if (t4_tick++ >= SMS_LISTEN_TIMEOUT)    // (uint16_t)(ee_read(0x00,sms_timeout)*1830)
            {
                t4_tick = 0;
                disable_tmr4();
                state = OPERATIONAL;
            }
            break;
        case LEARN_SENSOR:
            if (t4_tick++ >= LEARN_SENSOR_TIMEOUT)
            {
                t4_tick = 0;
                disable_tmr4();
                state = OPERATIONAL;
            }
            break;
        case DEL_SENSOR:
            if (t4_tick++ >= DEL_SENSOR_TIMEOUT)
            {
                t4_tick = 0;
                disable_tmr4();
                state = OPERATIONAL;
            }            
    }
}


void enable_tmr4()
{
    TMR4 = 0x00;
    T4CONbits.T4CKPS = 0b10;        // 1:16 pre-scaler
    T4CONbits.T4OUTPS = 0b1111;     // 1:16 post-scaler
    T4CONbits.TMR4ON = 1;
    PIE5bits.TMR4IE = 1;
    PIR5bits.TMR4IF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void disable_tmr4()
{
    PIE5bits.TMR4IE = 0;
    PIR5bits.TMR4IF = 0;
    T4CONbits.TMR4ON = 0;
    TMR4 = 0x00;
}


void disable_tmr5()
{
    tmr5_cnt = 0;
    T5CONbits.TMR5ON = 0;
    PIE5bits.TMR5IE = 0;
}


void reload_tmr5_pt25s()
{
    INTCONbits.GIE = 0;
    T5CON = 0b00110101;     // 1:8 prescale, clk src is internal instruction cycle,
                            // get value by two 8-bit reads, timer5 ON
    PIE5bits.TMR5IE = 1;
    INTCONbits.PEIE = 1;
    PIR5bits.TMR5IF = 0;
    TMR5H = 0x0B;
    TMR5L = 0x45;
    INTCONbits.GIE = 1;
}


void clear_at_ok()
{
    at1 = 0;
    at2 = 0;
    at3 = 0;
    at4 = 0;
    at5 = 0;
    at6 = 0;
    at7 = 0;
    at8 = 0;
    at9 = 0;
    at10 = 0;
    at11 = 0;
    at12 = 0;
}
