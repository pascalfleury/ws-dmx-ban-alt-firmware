#include <mcs51/lint.h>
#include "board.h"
#include "uart.h"
#include "dip.h"
#include "dmx.h"
#include "config.h"
#include "leds.h"

extern unsigned char ledBrightness[NUM_LEDS]; //defined in leds.c

//used to clicker the power led
unsigned char pwrLedCnt = 0;

inline void flickerPwrLed()
{
    //if power led has been turned off by uart, leave it off for 255 loop iterations
    if(PWR_LED)
    {
        pwrLedCnt++;
        if(pwrLedCnt == 255)
        {
            //turn on power led 
            //uart turns it off when it has received a correct frame.
            //this results in flickering if dmx is present and steady on if no dmx present.
            PWR_LED = 0;
            pwrLedCnt = 0;
        }
    }
}


void main()
{
    DmxState dmxState;

    dipInit();

    dmxInit(&dmxState);

    uartInit(); //initially sets AUXR
    ledInit(); //modifies AUXR

    PWR_LED = 0; //turn on power

    while(1)
    {
        flickerPwrLed();
        dmxUpdate(&dmxState);

        /* TODO: use dmxState.dimmer, dmxState.colorTemp,
         *       dmxState.strobeMode, dmxState.strobeSpeed
         *       to drive PWM outputs and LED brightness */
    }
}
