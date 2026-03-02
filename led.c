#include "led.h"
#include "board.h"

/* Error blink pattern as a bitmask, played MSB first (left-to-right).
 * Each bit represents one time slot of ERROR_SLOT_DURATION loop iterations.
 * 1 = LED on, 0 = LED off.
 *
 * Binary: 1010 1000 0000 0000
 *         ^^^^ ^
 *         3 blinks then pause
 *
 * Hex:    0xA800
 */
#define ERROR_SLOT_DURATION 5000   /* ~50ms per slot at ~10µs/iteration */
#define ERROR_PATTERN       0xA800
#define ERROR_PATTERN_LEN   16

/* Timeout: turn LED off if no frame for this many iterations (~500ms) */
#define DMX_LED_TIMEOUT 50000

static unsigned char slotIdx = 0;
static unsigned short slotCount = 0;
static unsigned long noFrameCount = 0;

void ledInit(void)
{
  DMX_LED = 0;
  slotIdx = 0;
  slotCount = 0;
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
    /* Set LED based on current bit in the pattern (MSB first) */
    DMX_LED = (ERROR_PATTERN >> (ERROR_PATTERN_LEN - 1 - slotIdx)) & 1;

    slotCount++;
    if (slotCount >= ERROR_SLOT_DURATION) {
      slotCount = 0;
      slotIdx++;
      if (slotIdx >= ERROR_PATTERN_LEN) {
        slotIdx = 0;
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
    slotIdx = 0;
    slotCount = 0;
  }
}
