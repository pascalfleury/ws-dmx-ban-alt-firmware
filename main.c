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
 * - Normal: LED toggles on each DMX frame (~44fps = visible flicker).
 *           If no frames arrive for ~500ms, LED turns off.
 * - Error:  LED flashes in a recognizable pattern (e.g. address invalid).
 *
 * Error pattern: 3 fast blinks followed by a pause.
 * Each blink is ~50ms on, ~50ms off. Pause is ~400ms.
 * Timing assumes ~10µs per main loop iteration at 24MHz.
 */

#define ERROR_BLINK_ON    5000   /* ~50ms at 10µs/iteration */
#define ERROR_BLINK_OFF   5000   /* ~50ms */
#define ERROR_PAUSE       40000  /* ~400ms */
#define ERROR_NUM_BLINKS  3

#define ERROR_CYCLE_LEN \
  (ERROR_NUM_BLINKS * (ERROR_BLINK_ON + ERROR_BLINK_OFF) + ERROR_PAUSE)

/* Timeout: turn LED off if no frame for this many iterations (~500ms) */
#define DMX_LED_TIMEOUT 50000

static unsigned long dmxLedCounter = 0;
static unsigned long dmxLedNoFrameCount = 0;

/* Called when a valid DMX frame is received.
 * Toggles the LED to create visible flicker at frame rate. */
static void dmxLedOnFrame(void)
{
  DMX_LED = !DMX_LED;
  dmxLedNoFrameCount = 0;
}

/* Update the DMX LED state.
 * In normal mode: track time since last frame, turn off if timeout.
 * In error mode: flash a recognizable pattern. */
static void dmxLedUpdate(unsigned char hasError)
{
  if (hasError) {
    /* Error pattern: 3 fast blinks + pause */
    unsigned long pos = dmxLedCounter % ERROR_CYCLE_LEN;
    unsigned long blinkPhase = ERROR_BLINK_ON + ERROR_BLINK_OFF;
    unsigned long blinkEnd = ERROR_NUM_BLINKS * blinkPhase;

    if (pos < blinkEnd) {
      /* In blink region */
      unsigned long withinBlink = pos % blinkPhase;
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
    /* Normal mode: if no frames for timeout period, turn LED off */
    dmxLedNoFrameCount++;
    if (dmxLedNoFrameCount > DMX_LED_TIMEOUT) {
      DMX_LED = 0;
      dmxLedNoFrameCount = DMX_LED_TIMEOUT; /* clamp to prevent overflow */
    }
    dmxLedCounter = 0; /* reset error pattern counter */
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
