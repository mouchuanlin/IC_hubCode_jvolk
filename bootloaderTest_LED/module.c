/* 
 * File:   module.c
 * Author: Jennifer
 * 
 * This file contains all functions related to general module configuration 
 * and operation.
 *
 * Created on April 25, 2018, 12:33 PM
 */


#include "config.h"
#include "module.h"
#include "uart.h"


bool startup_md_ok()
{
    init_uart_modem();
    if (!wakeup_module_ok())
        return false;
    if (!md_io_setup_ok())
        return false;
    if (!registration_ok())
        return false;
    if (!is_connected())
        return false;
//    if (!open_connection())
//    {
//        // Do something...
//    }
    
    return true;
}


void run_modem()
{
//    while (!queue.isEmpty())
//    {
//        
//    }
}