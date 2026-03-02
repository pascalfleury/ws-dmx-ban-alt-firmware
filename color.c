#include "color.h"
#include "numeric.h"
#include "config.h"

/* Color temperatures for each LED channel, defined in config.
 * Must be in ascending order (warmest to coldest). */
static const unsigned short colorTemps[NUM_LED_CHANNELS] = LED_COLOR_TEMPERATURES;

/* Scale a 16-bit value by a 16-bit factor.
 * Returns (a * b) >> 16, i.e. treats both as 0.16 fixed point
 * and returns a 0.16 fixed point result. */
static unsigned short scale16(unsigned short a, unsigned short b)
{
  unsigned long result = (unsigned long)a * b;
  return (unsigned short)(result >> 16);
}

void colorCompute(const DmxState *state, PwmState *pwm)
{
  unsigned short colorTemp = state->colorTemp;
  unsigned short dimmer = state->dimmer;
  unsigned char i;
  unsigned char zone;
  unsigned short zoneSize;
  unsigned short posInZone;
  unsigned short fade;

  /* Initialize all PWM channels to 0.
   * This ensures unused channels (when NUM_LED_CHANNELS < NUM_PWM_CHANNELS)
   * are explicitly off. */
  for (i = 0; i < NUM_PWM_CHANNELS; i++) {
    pwm->channel[i] = 0;
  }

#if NUM_LED_CHANNELS == 1
  /* Single LED channel: just apply dimmer */
  pwm->channel[0] = dimmer;
#else
  /* Number of zones = NUM_LED_CHANNELS - 1 */
  /* Each zone spans (0x10000 / numZones) of the colorTemp range */
  zoneSize = 0xFFFF / (NUM_LED_CHANNELS - 1);

  /* Determine which zone we're in */
  zone = colorTemp / zoneSize;

  /* Clamp to last zone */
  if (zone >= NUM_LED_CHANNELS - 1) {
    zone = NUM_LED_CHANNELS - 2;
  }

  /* Position within the zone: 0..zoneSize */
  posInZone = colorTemp - (unsigned short)zone * zoneSize;

  /* Fade: 0 at start of zone, 0xFFFF at end of zone */
  fade = (unsigned short)(((unsigned long)posInZone << 16) / zoneSize);

  /* Crossfade between channel[zone] and channel[zone+1].
   * Only LED channels 0..NUM_LED_CHANNELS-1 are used;
   * any remaining PWM channels stay at 0. */
  pwm->channel[zone]     = scale16(0xFFFF - fade, dimmer);
  pwm->channel[zone + 1] = scale16(fade, dimmer);
#endif
}
