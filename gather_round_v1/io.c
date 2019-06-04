
#include "config.h"
#include "modem.h"
#include "queue.h"
#include "uart.h"

bool buttonStillPressed = false;
bool waiting = true;

void init_pic()
{
        /* OSC */
    OSCCONbits.IDLEN = 0;
    OSCCONbits.IRCF = 0b110;    // 8MHz; 0b101 is 4MHz
    OSCCONbits.OSTS = 1;        // Clk defined by Fosc
//    OSCCON = 0x68;
    
    ANSELA = 0x01;
    ANSELB = 0x00;
    ANSELC = 0x00;
    
    TRISA = 0b00000001;
    TRISB = 0b10110110;
    TRISC = 0b10000100;
    
    B_OFF();
    G_OFF();
    
    
    /* INTERRUPTS */
    /* t0con = 0x87: enable timer0, use as 16-bit counter, transition on
     * internal instruction cycle, assign prescaler of: 1:256.
     * Timer0 used to control LEDs.
     */
    T0CON = TMR0_CFG;             //1*4000 = 50,000us
    TMR0H = ((65535-TMR0_HI_FREQ)/256);
    TMR0L = ((65535-TMR0_HI_FREQ)%256);
    TMR0IF = 0;
//    TMR0IE = 1;
//    
//    INTEDG1 = 1;            // Interrupt on rising edge of Alarm
//    INTEDG2 = 1;            // Interrupt on rising edge of "Error"/tamper
//    INT1IP = 1;             // Assign high priority to alarm
//    INT2IP = 1;             // Assign high priority to tamper
//    IOCBbits.IOCB4 = 1;     // Interrupt on Test signal
//    
//    INT1IE = 1;             // Enable Alarm interrupt
//    INT2IE = 1;             // Enable Tamper interrupt
//    INTCONbits.RBIE = 1;    // Enable Test interrupt
//    
//    ALARM_INT = 0;
//    TAMPER_INT = 0;
//    TEST_INT = 0;
//    
//    INTCONbits.PEIE = 1;    // Enable Low and High priority interrupts
//    INTCONbits.GIE = 1;     // Enable HIGH-priority interrupts only
    
//    ledPattern = STANDBY;
    
    LATAbits.LATA4 = 0;     // V_cap
    IOCBbits.IOCB4 = 1;
    
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;
    
    STATE = IDLE;
    MD_STATE = MD_OFF;
    
    
    init_uart();
    
    clear_at_ok();
    
    enable_ints();
}

void check_sensor()         // checks whether the tripped device is learned in; and if so, identifies the zone number
{
    if (receivedSensor)
    {
        uint8_t sensorIndex = getIndexOfLearnedSensor(rx2_buf);
        uint8_t zone;
        receivedSensor = false;
        if (sensorIndex != 0xFF)
        {
            zone = ID[sensorIndex][3];
            add_event(sensorIndex, zone);
        }
    }
}


void check_test()
{
    
    if (TEST && !buttonStillPressed)           // Check button
            {
                __delay_ms(30);         // De-bounce
                if (TEST)
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
                if (!TEST)
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
            
    
//    if (buttonPressed1)      // e.g. some button press combination '1'
//    {
//        PREV_STATE = STATE;     // save current state before overwriting
//        STATE = LISTEN_SMS;
//    }
//    else if (buttonPressed2)
//    {
//        start_sensor_timer();       // timer 4
//        PREV_STATE = STATE;     // save current state before overwriting
//        STATE = LEARN_SENSOR;
//    }
//    else if (buttonPressed3)
//    {
//        start_sensor_timer();
//        PREV_STATE = STATE;     // save current state before overwriting
//        STATE = DEL_SENSOR;
//    }
//    else if (buttonPressed4)
//        add_to_queue(TEST, ee_read(HUB_ZONE_ADDR));
}



void add_sensor(uint8_t sensorID[])
{
    bool isUnique = true;
    uint8_t zone = 1, dev_type_new, dev_type_existing;
    if (receivedSensor && (idEndPtr < (ID_LEN - 1)))
    {
        receivedSensor = false;
        send_uart_ack();
        for (uint8_t i = 0; i < MAX_SENSOR; i++)
        {
            isUnique &= (bool)((bool)(rx2_buf[0] != ID[i][0]) || (bool)(rx2_buf[1] != ID[i][1]) || \
                    (bool)(rx2_buf[2] != ID[i][2]));
            dev_type_new = (uint8_t)((rx2_buf[0] >> 4) & 0x0F);
            dev_type_existing = (uint8_t)((ID[i][0] >> 4) & 0x0F);
            if (dev_type_existing == dev_type_new)
                zone++;
        }
        if (isUnique)
        {
            for (uint8_t i = 0; i < ID_LEN - 1; i++)
            {
                ID[idEndPtr][i] = rx2_buf[i];
                ee_write(0x01, (uint8_t)(ID_START_ADDR + (ID_LEN * idEndPtr) + i), ID[idEndPtr][i]);
            }
            ID[idEndPtr][3] = zone;
            ee_write(0x01, (uint8_t)(ID_START_ADDR + (ID_LEN * idEndPtr) + 3), ID[idEndPtr][3]);
            idEndPtr++;
        }
    }
}


void del_sensor(uint8_t sensorID[])
{
    if (receivedSensor)
    {
        uint8_t sensorIndex = getIndexOfLearnedSensor(sensorID);
        uint8_t i, j;
        if (sensorIndex != 0xFF)
        {
            for (i = 0; i < ID_LEN; i++)
                ee_write(0x01, (uint8_t)(ID_START_ADDR + (sensorIndex * ID_LEN) + i), 0x00);     // write over existing entry with zeroes
            for (i = sensorIndex; i < (MAX_SENSOR - 1); i++)
            {
                for (j = 0; j < ID_LEN; j++)
                    ee_write(0x01, (uint8_t)(ID_START_ADDR + (sensorIndex * ID_LEN) + i), \
                            (ee_read(0x01, (uint8_t)(ID_START_ADDR + (sensorIndex * (ID_LEN + 1)) + j))));
            }
        }
    }
}



void load_ID()
{
    idEndPtr = ee_read(0x00, NUM_SENSORS_ADDR);
    for (uint8_t i = 0; i < idEndPtr; i++)
    {
        for (uint8_t j = 0; j < ID_LEN; j++)
            ID[i][j] = ee_read(0x01, (uint8_t)(ID_START_ADDR + j + i));        // one after the other for now
    }
}


uint8_t getIndexOfLearnedSensor(uint8_t sensorID[])
{
    bool isLearnedIn = false;
    uint8_t i = 0, sensorIndex;
    if (receivedSensor)
    {
        receivedSensor = false;
        send_uart_ack();
        while (i < MAX_SENSOR && !isLearnedIn)
        {
            isLearnedIn = true;
            for (uint8_t j = 0; j < ID_LEN - 1; j++)
            {
                isLearnedIn &= (bool)(rx2_buf[j] == ID[i][j]);
            }
            if (isLearnedIn)
            {
                sensorIndex = i;
                return sensorIndex;
            }
            i++;
        }
    }
    return 0xFF;            // Not learned in
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

void disable_tmr5()
{
    tmr5_cnt = 0;
    T5CONbits.TMR5ON = 0;
    PIE5bits.TMR5IE = 0;
}

void start_sensor_tmr()     // for learning mode timeout
{
    t4_tick = 0;
    T4CONbits.T4CKPS = 0b10;        // 1:16 pre-scale
    T4CONbits.T4OUTPS = 0b1111;     // 1:16 post-scale
    T4CONbits.TMR4ON = 1;
    PIE5bits.TMR4IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void stop_sensor_tmr()
{
    T4CONbits.TMR4ON = 0;
    PIE5bits.TMR4IE = 0;
}


void check_super()
{
    
}

void check_tamper()
{
    
}

void check_alarm()
{
    
}

void enable_ints()
{
    INTCONbits.GIE = 0;
    
//    /* Test Button */ - NOW POLLED - SEE STATE.C
//    IOCBbits.IOCB4 = 1;     // Interrupt on Test signal
//    INTCONbits.RBIE = 1;    // Enable Test interrupt
    
    /* Timer 0, used for LEDs */
    start_timer0();                 // timer0 is inactive during sleep
    
    /* Timer 3 */
    T3CONbits.T3CKPS = 0b11;        // 1:8 prescale
    T3CONbits.TMR3CS = 0b00;        // Runs off of instruction cycle (Fosc/4)
    T3CONbits.nT3SYNC = 1;
    
    T3CONbits.TMR3ON = 1;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    TMR3H = 0x60;
    TMR3L = 0;
    
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;
    
    INTCONbits.GIE = 1;
}

//void reload_tmr3_2s()
//{
//    T3CONbits.TMR3ON = 1;
//    PIE2bits.TMR3IE = 1;
//    PIR2bits.TMR3IF = 0;
//    TMR3H = 0x30;
//    TMR3L = 0;
//}

void reload_tmr3_pt25s()
{
    T3CONbits.TMR3ON = 1;
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    TMR3H = 0x0B;
    TMR3L = 0x45;
}

void disable_tmr3()
{
    buttonPressCount = 0;
    tmr3_cnt = 0;
    T3CONbits.TMR3ON = 0;
    PIE2bits.TMR3IE = 0;
}

void start_timer0()
{
    T0CON = 0x87;             //1*4000 = 50,000us
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
    TMR0IF = 0;
    TMR0IE = 1;
    INTCONbits.GIE = 1;
}


void reload_timer0()
{
    TMR0L = ((65535-_100milliseconds)%256);//for 8MHz
    TMR0H = ((65535-_100milliseconds)/256);
}



void control_leds()
{
    switch (STATE)
    {
        case IDLE:
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
            
        case DEL_SENSOR:
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