#include <mcs51/lint.h>
#include "board.h"
#include "uart.h"
#include "dip.h"
#include "dmx.h"
#include "config.h"
#include "leds.h"
#include "led.h"
#include "pwm.h"
#include "color.h"



void main()
{
  DmxState dmxState;
  PwmState pwmState;

  dipInit();
  pwmInit();
  uartInit(); //initially sets AUXR
  ledsInit(); //modifies AUXR
  ledInit();

  dmxInit(&dmxState);
  pwmStateInit(&pwmState);

  while (1) {
    if (dmxAddressValid()) {
      /* Address is valid: process DMX data normally */
      if (dmxUpdate(&dmxState)) {
        ledOnFrame();
        colorCompute(&dmxState, &pwmState);
        pwmUpdate(&pwmState);
      }
      ledUpdate(0);
    } else {
      /* Address is invalid: turn off all outputs, blink error */
      pwmStateInit(&pwmState);
      pwmUpdate(&pwmState);
      ledUpdate(1);
    }
  }
}
