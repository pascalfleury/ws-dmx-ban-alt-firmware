#include "dip.h"
#include <mcs51/lint.h>
#include "board.h"
#include "config.h"

/* DIP switch mapping is defined in the board header */

extern unsigned short dmxAddr; //defined in uart.c

void dipInit()
{
    //we use the quasi-bi-directional mode.
    //This is the default thus we dont have to configure the mode.

    //turn on the pullups
    DIP_1 = 1;
    DIP_2 = 1;
    DIP_3 = 1;
    DIP_4 = 1;
    DIP_5 = 1;
    DIP_6 = 1;
    DIP_7 = 1;
    DIP_8 = 1;
    DIP_9 = 1;
    DIP_10_MODE = 1;
}

unsigned short readDmxAddr()
{
    //initialize unused bits as 1 (will later be inverted to 0)
    unsigned short result = 0xfe00;

    result = result
        | DIP_1
        | (DIP_2 << 1)
        | (DIP_3 << 2)
        | (DIP_4 << 3)
        | (DIP_5 << 4)
        | (DIP_6 << 5)
        | (DIP_7 << 6)
        | (DIP_8 << 7)
        | (DIP_9 << 8);

    //due to the pullups we read a 1 when the dip is in off-position.
    //thus invert every pin
    result = ~result;
    return result;
}

unsigned char readFunctionDip()
{
    return !DIP_10_MODE;
}

void readDipSwitch()
{
    dmxAddr = readDmxAddr();

    if(dmxAddr == 0)
    {
        dmxAddr = 1;
    }
    if(dmxAddr > 512 - NUM_ADRESSES)
    {
        dmxAddr = 512 - NUM_ADRESSES;
    }
}
