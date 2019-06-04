/* 
 * File:   uart.h
 * Author: Scott
 *
 * Created on June 12, 2018, 3:37 PM
 */

#ifndef UART_H
#define	UART_H

/*****************************************************
 * INCLUDES
 ****************************************************/
#include "config.h"


/*****************************************************
 * DEFINES
 ****************************************************/
#define ERR_CODE_TIMED_OUT          0x01
#define ERR_CODE_REC_ERROR          0x02
#define ERR_CODE_REC_OK             0x03
#define ERR_CODE_RETRIES_ENDED      0x04
#define ERR_CODE_NA                 0x05        // Just to avoid compiler warning for now



/*****************************************************
 * FUNCTION PROTOTYPES
 ****************************************************/
void init_md_uart();
void terminate_md_uart();
uint8_t md_uart_write_str_ok(unsigned char *data, uint8_t len1, \
        bool wantOK, bool waitToSendMoreData);       // Returns ERR Codes as defined above
void md_uart_send(unsigned char data);
void enable_md_rxint();
void disable_md_rxint();

/*****************************************************
 * VARIABLES
 ****************************************************/
enum AT_CMD_STATE {
    AT_SEND,
    AT_WAIT_MORE,
    AT_WAIT_OK,
    AT_RESET
};



enum AT_CMD_STATE atCmdState;

uint8_t mdRxBuf[100];                           // allocate bytes ahead of time
uint8_t endPtr = 0;
int8_t numRetries = 15;
bool receivedOK, receivedERR, receivedRESP, receivedCarat, timedOut;

    uint8_t referenceBuf[60];

#endif	/* UART_H */

