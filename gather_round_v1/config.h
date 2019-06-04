/* 
 * File:   config.h
 * Author: Scott
 *
 * Created on September 14, 2018, 8:21 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

// PIC18F26K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = SWON//OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = INTMCLR  // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = ON        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = ON        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = ON        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = ON        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = ON        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = ON        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)


// INCLUDES
#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

// DEFINES
#define _XTAL_FREQ      8000000


#define MD_PWR          LATAbits.LATA3
#define MD_RESET        LATAbits.LATA1
#define SELECT          RC1     // UNUSED IN THIS CODE
#define TRIS_WIFI_TX    TRISBbits.TRISB7        // pic RX
#define TRIS_WIFI_RX    TRISBbits.TRISB6        // pic TX
#define TRIS_MD_TX      TRISCbits.TRISC7        // pic RX
#define TRIS_MD_RX      TRISCbits.TRISC6        // pic TX
#define TRIS_WIFI_INT   TRISBbits.TRISB5

#define B_LED           !LATCbits.LATC4
#define G_LED           !LATBbits.LATB3
#define TEST            PORTBbits.RB4
#define ERROR_PIN       RB2
#define ALARM           RB1
#define STANDBY         RB0
#define WIFI_INT        RB5

#define B_ON()          LATC4=0
#define B_OFF()         LATC4=1
#define G_ON()          LATB3=0
#define G_OFF()         LATB3=1

#define MAX_SENSOR      16
#define ID_LEN          4

#define OFF             0
#define ON              1
#define YES             true
#define NO              false

#define _T5_45s             (4*45)
#define _T5_10s             (4*10)
#define _T5_2s              (4*2)

#define _T2_3s              31      // with 1:16 pre- and post-scaler

#define TMR0_CFG            0x87
#define TMR0_LO_FREQ        360
#define TMR0_HI_FREQ        780     //4M->390

#define _100milliseconds    781//22500       //orig. 780. Calibrated for Timer0 100ms interval for 8MHz clk


#define SENSOR_MODE_TIMEOUT 611     // ticks to get to 1min with 1:16 pre- and post-scale on timer 4

// FUNCTION PROTOS
void init_pic();
void init_uart();
void load_ID();
void check_state();
void check_super();
void check_tamper();
void check_alarm();
void check_test();
void check_sensor();
//void check_event();
void start_sms_timer();
void stop_sms_timer();
void listen_sms();
void clear_at_ok();
void reload_tmr5_pt25s();
void disable_tmr5();
uint8_t start_sms_ok();
uint8_t send_sms_ok(bool isValidCmd);
uint8_t read_sms();
bool program_field(uint8_t field_number, uint8_t data_buf[], uint8_t size);

void enable_ints();
void reload_tmr3_2s();
void reload_tmr3_pt25s();
void disable_tmr3();
void start_timer0();
void reload_timer0();

void control_leds();

void start_sensor_tmr();
void stop_sensor_tmr();
void add_sensor(uint8_t sensorID[]);
void del_sensor(uint8_t sensorID[]);

uint8_t ee_read(uint8_t page, uint8_t addr);
void ee_write(uint8_t page, uint8_t addr, uint8_t data);
uint8_t ee_read_ascii_to_value(uint8_t page, uint8_t addr);

uint8_t getIndexOfLearnedSensor(uint8_t sensorID[]);

// VARIABLES
enum HUB_STATE
{
    IDLE,           // sleep
    AWAKE,          // LEDs on for button presses
    LISTEN_SMS,
    LEARN_SENSOR,
    DEL_SENSOR,
};
enum HUB_STATE STATE;
enum HUB_STATE PREV_STATE;

enum LED_MODE {
    LED_OFF,
    STANDBY,
    INTERNET,
    APN_IP_NOT_SET,
    NETWORK_ERR,
    NETWORK_REGISTER
};

enum LED_MODE ledPattern;

uint8_t ledPatternCount = 0, tmr5_cnt;
uint8_t ID[MAX_SENSOR][ID_LEN];      // 4 fields for each peripheral: 3 for serial ID in hex, 1 for learned zone
uint8_t idEndPtr = 0;
uint8_t rx2_buf[5];
uint8_t rx2pos;

bool sms_timer_ready = false;
bool receivedSensor = false;
bool sms_echo_ready = false, sms_err_ready = false;
bool ready_to_read_text = false;
uint8_t test_count = 0;
uint8_t sms_tmr_tick = 0, t2_tick = 0;
uint16_t t4_tick;
uint8_t smsMsgIndex;
uint8_t currentChar;


bool inButtonMenu = false;
uint8_t buttonPressCount = 0;
uint8_t gled_tmr0_tick, bled_tmr0_tick, tmr3_cnt;

#endif	/* CONFIG_H */

