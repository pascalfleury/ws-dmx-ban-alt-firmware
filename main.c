#include <mcs51/lint.h>
#include "board.h"
#include "uart.h"
#include "dip.h"
#include "dmx.h"
#include "config.h"
#include "leds.h"
#include "pwm.h"
#include "color.h"

/* DMX LED modes:
 * - Normal: LED flickers when DMX data is received, off when no data.
 * - Error:  LED flashes in a recognizable pattern (e.g. address clamped).
 *
 * Error pattern: 3 fast blinks followed by a pause.
 * Each blink is ~50ms on, ~50ms off. Pause is ~400ms.
 * At ~1 iteration per loop pass, we use a counter to time this.
 */

#define ERROR_BLINK_ON    50
#define ERROR_BLINK_OFF   50
#define ERROR_PAUSE       400
#define ERROR_NUM_BLINKS  3

/* Total length of one error pattern cycle */
#define ERROR_CYCLE_LEN \
  (ERROR_NUM_BLINKS * (ERROR_BLINK_ON + ERROR_BLINK_OFF) + ERROR_PAUSE)

static unsigned short dmxLedCounter = 0;
static unsigned short dmxLedTimeout = 0;

/* Called when a valid DMX frame is received */
static void dmxLedOnFrame(void)
{
  /* Brief flash: turn LED on, reset timeout */
  DMX_LED = 1;
  dmxLedTimeout = 0;
  dmxLedCounter = 0;
}

/* Update the DMX LED state.
 * In normal mode: LED was turned on by dmxLedOnFrame(),
 * turn it off after a short time. If no frames arrive,
 * LED stays off.
 * In error mode: flash a recognizable pattern. */
static void dmxLedUpdate(unsigned char hasError)
{
  if (hasError) {
    /* Error pattern: 3 fast blinks + pause */
    unsigned short pos = dmxLedCounter % ERROR_CYCLE_LEN;
    unsigned short blinkPhase = ERROR_BLINK_ON + ERROR_BLINK_OFF;
    unsigned short blinkEnd = ERROR_NUM_BLINKS * blinkPhase;

    if (pos < blinkEnd) {
      /* In blink region */
      unsigned short withinBlink = pos % blinkPhase;
      if (withinBlink < ERROR_BLINK_ON) {
        DMX_LED = 1;
      } else {
        DMX_LED = 0;
      }
    } else {
      /* In pause region */
      DMX_LED = 0;
    }

    dmxLedCounter++;
    if (dmxLedCounter >= ERROR_CYCLE_LEN) {
      dmxLedCounter = 0;
    }
  } else {
    /* Normal mode: turn off LED after a short delay.
     * dmxLedOnFrame() turns it on when data arrives. */
    if (DMX_LED) {
      dmxLedTimeout++;
      if (dmxLedTimeout > 2) {
        DMX_LED = 0;
        dmxLedTimeout = 0;
      }
    }
  }
}


void main()
{
  DmxState dmxState;
  PwmState pwmState;

  dipInit();
  pwmInit();
  uartInit(); //initially sets AUXR
  ledInit(); //modifies AUXR

  dmxInit(&dmxState);
  pwmStateInit(&pwmState);

  DMX_LED = 0;

  while (1) {
    if (dmxAddressValid()) {
      /* Address is valid: process DMX data normally */
      if (dmxUpdate(&dmxState)) {
        dmxLedOnFrame();
        colorCompute(&dmxState, &pwmState);
        pwmUpdate(&pwmState);
      }
      dmxLedUpdate(0);
    } else {
      /* Address is invalid: turn off all outputs, blink error */
      pwmStateInit(&pwmState);
      pwmUpdate(&pwmState);
      dmxLedUpdate(1);
    }
  }
}
