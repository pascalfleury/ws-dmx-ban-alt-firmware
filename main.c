#include <mcs51/lint.h>
#include "board.h"
#include "delay.h"
#include "uart.h"
#include "dip.h"
#include "config.h"
#include "leds.h"

extern volatile unsigned char dmxData[NUM_ADRESSES]; //defined in uart.c
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
    unsigned short masterBrightness = 0;

    dipInit();

    readDipSwitch();

    uartInit(); //initially sets AUXR
    ledInit(); //modifies AUXR

    PWR_LED = 0; //turn on power

    while(1)
    {
        flickerPwrLed();
        readDipSwitch();

        masterBrightness = dmxData[0];

        // The master scaling is done in fixed point math with scale 255
        // 255 was chosen because it allows to ommit scaling of masterBrightness
        // and is close to the theoretical maximum scale of 257 
        // (255*257=biggest possible unsigned short).

        //loop unrolled for performance reasons
        ledBrightness[0] = (dmxData[2] * masterBrightness) / 255;
        ledBrightness[1] = (dmxData[3] * masterBrightness) / 255;
        ledBrightness[2] = (dmxData[4] * masterBrightness) / 255;
        ledBrightness[3] = (dmxData[5] * masterBrightness) / 255;
        ledBrightness[4] = (dmxData[6] * masterBrightness) / 255;
        ledBrightness[5] = (dmxData[7] * masterBrightness) / 255;
        ledBrightness[6] = (dmxData[8] * masterBrightness) / 255;
        ledBrightness[7] = (dmxData[9] * masterBrightness) / 255;
    }
}
