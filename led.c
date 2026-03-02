#include "led.h"
#include "board.h"

/* Error blink pattern as a bitmask.
 * Each bit represents one time slot of ERROR_SLOT_DURATION
 * loop iterations. Bit 0 (LSB) is played first.
 * 1 = LED on, 0 = LED off.
 *
 * Pattern: 3 fast blinks then a pause
 *   0b00000000 00010101 = 0x0015
 *   slot 0: ON   (blink 1 on)
 *   slot 1: OFF  (blink 1 off)
 *   slot 2: ON   (blink 2 on)
 *   slot 3: OFF  (blink 2 off)
 *   slot 4: ON   (blink 3 on)
 *   slots 5-15: OFF (pause)
 */
#define ERROR_PATTERN       0x0015
#define ERROR_PATTERN_LEN   16
#define ERROR_SLOT_DURATION 5000  /* ~50ms per slot at ~10µs/iteration */

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
    /* Set LED based on current bit in the pattern */
    DMX_LED = (ERROR_PATTERN >> slotIdx) & 1;

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
