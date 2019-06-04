/* 
 * File:   config.h
 * Author: Scott
 *
 * Created on April 24, 2018, 3:11 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

/*****************************************************
 * CONFIGURATION BITS
 ****************************************************/
// CONFIG1H
//#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config FOSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = ON      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF//SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF//SWON     // Watchdog Timer Enable bits (Watch dog enabled in software.). Nominal WDT period is 4ms, multiplied by 16-bit postscaler. 
                                // Output of the WDT postscaler selected by multiplexer & controlled by WDTPS bits.
#pragma config WDTPS = 256    // Watchdog Timer Postscale Select bits (1:32768). Max, 32768, corresponds to 131.072s, or 2.18min.

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
#pragma config CP0 = OFF//N        // Code Protection Block 0 (Block 0 (000800-003FFFh) code-protected)
#pragma config CP1 = OFF//N        // Code Protection Block 1 (Block 1 (004000-007FFFh) code-protected)
#pragma config CP2 = OFF//N        // Code Protection Block 2 (Block 2 (008000-00BFFFh) code-protected)
#pragma config CP3 = OFF//N        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) code-protected)

// CONFIG5H
#pragma config CPB = OFF//N        // Boot Block Code Protection bit (Boot block (000000-0007FFh) code-protected)
#pragma config CPD = OFF//N        // Data EEPROM Code Protection bit (Data EEPROM code-protected)

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
#include "module.h"

/*****************************************************
 * DEFINES
 ****************************************************/

#endif	/* CONFIG_H */

