/* 
 * File:   modem.h
 * Author: JV
 *
 * Created on June 12, 2018, 4:20 PM
 */

#ifndef MODEM_H
#define	MODEM_H

enum MODEM_STATE {
    IDLE,
    WAITING_FOR_BOOT,
    CONFIG,
    SET_UP_DATA,
    WAITING_PDP_ACTIVE,
    SENDING_DATA,
    WAITING_ACK,
    SET_UP_SMS,
    SENDING_SMS
};

enum MODEM_STATE md_state;

void start_modem_send_process();
void check_modem_state();
void start_md_boot_timer();
bool send_md_cfg_cmds_ok();
bool setup_data_ok();
bool send_over_socket_ok();
bool close_socket_ok();



bool bootTimerIsDone, pdpIsActive;

#endif	/* MODEM_H */

