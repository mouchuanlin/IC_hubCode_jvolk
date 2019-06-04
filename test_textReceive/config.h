/* 
 * File:   config.h
 * Author: Jennifer
 *
 * Created on April 4, 2018, 5:36 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

/*****************************************************
 * CONFIGURATION BITS
 ****************************************************/
// CONFIG1H
//#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config FOSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
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
#pragma config CP0 = ON        // Code Protection Block 0 (Block 0 (000800-003FFFh) code-protected)
#pragma config CP1 = ON        // Code Protection Block 1 (Block 1 (004000-007FFFh) code-protected)
#pragma config CP2 = ON        // Code Protection Block 2 (Block 2 (008000-00BFFFh) code-protected)
#pragma config CP3 = ON        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) code-protected)

// CONFIG5H
#pragma config CPB = ON        // Boot Block Code Protection bit (Boot block (000000-0007FFh) code-protected)
#pragma config CPD = ON        // Data EEPROM Code Protection bit (Data EEPROM code-protected)

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



/*****************************************************
 * INCLUDES
 ****************************************************/
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


/*****************************************************
 * DEFINES
 ****************************************************/
#define OUTPUT  0
#define INPUT   1

#define YES     1
#define NO      0
   
//PORTA
#define BAT_DECT            PORTAbits.RA0
#define BAT_DECT_TRIS       TRISAbits.TRISA0
    
#define MD_RESET            PORTAbits.RA1
#define MD_RESET_TRIS       TRISAbits.TRISA1

#define MD_POWER            PORTAbits.RA3       //LATA3 is not available, no latch for RA3... hardware bug
#define MD_POWER_TRIS       TRISAbits.TRISA3

//PORTC
#define B_LED               PORTCbits.RC4       // Illuminates when RC4 goes low. Use WPU on this pin
#define B_LED_TRIS          TRISCbits.TRISC4
#define TX_MD               PORTCbits.RC6
#define TX_MD_TRIS          TRISCbits.TRISC6
#define RX_MD               LATCbits.LATC7
#define RX_MD_TRIS          TRISCbits.TRISC7
//#define SELECT              LATCbits.LATC1      // Not used
#define SELECT_TRIS         TRISCbits.TRISC1
    
//PORTB
//#define STANDBY_PIN         PORTBbits.RB0     // Not used
#define STANDBY_PIN_TRIS    TRISBbits.TRISB0    
#define ALARM_PIN           PORTBbits.RB1
#define ALARM_PIN_TRIS      TRISBbits.TRISB1     
#define ERROR_PIN           PORTBbits.RB2
#define ERROR_PIN_TRIS      TRISBbits.TRISB2
#define G_LED               LATBbits.LATB3      // Illuminates when RB3 goes low. Use WPU on this pin
#define G_LED_TRIS          TRISBbits.TRISB3

#define TEST_PIN            PORTBbits.RB4
#define TEST_PIN_TRIS       TRISBbits.TRISB4
#define WIFI_INT            PORTBbits.RB5
#define WIFI_INT_TRIS       TRISBbits.TRISB5
#define TX_WIFI             LATBbits.LATB6
#define TX_WIFI_TRIS        TRISBbits.TRISB6
#define RX_WIFI             PORTBbits.RB7
#define RX_WIFI_TRIS        TRISBbits.TRISB7

#define TMR0_CFG            0x87
#define TMR0_LO_FREQ        360
#define TMR0_HI_FREQ        780     //4M->390

#define TAMPER_INT          INTCON3bits.INT2IF
#define ALARM_INT           INTCON3bits.INT1IF
#define TEST_INT            INTCONbits.RBIF

/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void init_pic();
void setup_timer3();
void check_leds();

/*****************************************************
 * VARIABLES
 ****************************************************/
const uint8_t VERSION[]= "2000";                   // Translates to version 2.00

enum LED_MODE {
    OFF,
    STANDBY,
    INTERNET,
    APN_IP_NOT_SET,
    NETWORK_ERR,
    NETWORK_REGISTER
};

enum LED_MODE ledPattern;

uint8_t ledPatternCount = 0;



#endif	/* CONFIG_H */