
#include "config.h"
#include "modem.h"

void start_modem()
{
    if (!modemAwake)
    {
        MD_PWR = 1;
    }
    else
    {
        disable_md_uart();
        MD_PWR = 0;      // or MD_RESET if used
        __delay_ms(200);
        MD_PWR = 1;
    }
    tmr3_cnt = 0;
    reload_tmr3_pt25s();        // start timer3 for modem boot
    waitingForModemBoot = true;
}




