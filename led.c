#include "led.h"
#include "board.h"

/* Error pattern: {on, off, on, off, ..., 0}
 * Each value is a duration in loop iterations (~10µs each at 24MHz).
 * LED starts ON for the first value, OFF for the second, etc.
 * A 0 terminates the pattern and it restarts.
 * Pattern: 3 fast blinks (50ms on, 50ms off) then a long pause (450ms). */
static const unsigned short errorPattern[] = {
  5000,   /* ON  ~50ms */
  5000,   /* OFF ~50ms */
  5000,   /* ON  ~50ms */
  5000,   /* OFF ~50ms */
  5000,   /* ON  ~50ms */
  45000,  /* OFF ~450ms */
  0       /* end of pattern, restart */
};

/* Timeout: turn LED off if no frame for this many iterations (~500ms) */
#define DMX_LED_TIMEOUT 50000

static unsigned char patternIdx = 0;
static unsigned short patternCount = 0;
static unsigned long noFrameCount = 0;

void ledInit(void)
{
  DMX_LED = 0;
  patternIdx = 0;
  patternCount = 0;
  noFrameCount = 0;
}

void ledOnFrame(void)
{
  DMX_LED = !DMX_LED;
  noFrameCount = 0;
}

void ledUpdate(unsigned char hasError)
{
  if (hasError) {
    /* Set LED state: even indices = ON, odd indices = OFF */
    DMX_LED = !(patternIdx & 1);

    patternCount++;
    if (patternCount >= errorPattern[patternIdx]) {
      patternCount = 0;
      patternIdx++;
      if (errorPattern[patternIdx] == 0) {
        patternIdx = 0;
      }
    }
  } else {
    /* Normal mode: if no frames for timeout period, turn LED off */
    noFrameCount++;
    if (noFrameCount > DMX_LED_TIMEOUT) {
      DMX_LED = 0;
      noFrameCount = DMX_LED_TIMEOUT; /* clamp to prevent overflow */
    }
    /* Reset error pattern so it starts fresh if error recurs */
    patternIdx = 0;
    patternCount = 0;
  }
}
