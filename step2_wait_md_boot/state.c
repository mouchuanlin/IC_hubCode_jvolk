
#include "config.h"
#include "modem.h"
#include "state.h"

void control_leds()
{
    switch (STATE)
    {
        case NREM:
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
//        B_OFF();
        bled_tmr0_tick = 0;
        break;
    }
}


void check_state()
{
    switch (STATE)
    {
        case NREM:
            if (modemMayBeAwake)
            {
                // Start sending AT commands
                // if initial AT commands are successful,
                // stop timer3, clear _25sTmr3Cnt, 
                // set waitingForModemBoot false, 
                // set modemAwake true.
            }
    }
}