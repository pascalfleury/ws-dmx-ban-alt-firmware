#include "color.h"
#include "numeric.h"
#include "board.h"

/* Color temperatures for each PWM channel, defined by the board.
 * Must be in ascending order (warmest to coldest). */
static const unsigned short colorTemps[NUM_PWM_CHANNELS] = PWM_COLOR_TEMPS;

/* Color temperature mapping using board-defined values.
 *
 * The colorTemp range 0x0000..0xFFFF is divided into (NUM_PWM_CHANNELS - 1) zones.
 * Each zone represents a crossfade between two adjacent PWM channels.
 *
 * Within each zone, we linearly interpolate between the two adjacent channels.
 * The result is then scaled by the dimmer value.
 *
 * All math is 16-bit or 32-bit integer, no floating point.
 */

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

  /* Initialize all channels to 0 */
  for (i = 0; i < NUM_PWM_CHANNELS; i++) {
    pwm->channel[i] = 0;
  }

#if NUM_PWM_CHANNELS == 1
  /* Single channel: just apply dimmer */
  pwm->channel[0] = dimmer;
  return;
#else
  /* Number of zones = NUM_PWM_CHANNELS - 1 */
  /* Each zone spans (0x10000 / numZones) of the colorTemp range */
  zoneSize = 0xFFFF / (NUM_PWM_CHANNELS - 1);

  /* Determine which zone we're in */
  zone = colorTemp / zoneSize;

  /* Clamp to last zone */
  if (zone >= NUM_PWM_CHANNELS - 1) {
    zone = NUM_PWM_CHANNELS - 2;
  }

  /* Position within the zone: 0..zoneSize */
  posInZone = colorTemp - (unsigned short)zone * zoneSize;

  /* Fade: 0 at start of zone, 0xFFFF at end of zone */
  fade = (unsigned short)(((unsigned long)posInZone << 16) / zoneSize);

  /* Crossfade between channel[zone] and channel[zone+1] */
  pwm->channel[zone]     = scale16(0xFFFF - fade, dimmer);
  pwm->channel[zone + 1] = scale16(fade, dimmer);
#endif
}
#include "color.h"
#include "numeric.h"
#include "config.h"

/* Color temperatures for each channel, defined in config.
 * Must be in ascending order (warmest to coldest). */
static const unsigned short colorTemps[NUM_COLOR_CHANNELS] = COLOR_TEMPS;

void colorCompute(const DmxState *dmxState, PwmState *pwmState)
{
  unsigned short dimmer = dmxState->dimmer;
  unsigned short colorTemp = dmxState->colorTemp;
  unsigned char zone;
  unsigned short zoneSize;
  unsigned short posInZone;
  unsigned long warm, cold;
  unsigned char i;

  /* Initialize all channels to 0 */
  for (i = 0; i < NUM_PWM_CHANNELS; i++) {
    pwm->channel[i] = 0;
  }

#if NUM_COLOR_CHANNELS == 1
  /* Single channel: just apply dimmer */
  pwm->channel[0] = dimmer;
  return;
#else
  /* Number of zones = NUM_COLOR_CHANNELS - 1 */
  /* Each zone spans (0x10000 / numZones) of the colorTemp range */
  zoneSize = 0xFFFF / (NUM_COLOR_CHANNELS - 1);

  /* Determine which zone we're in */
  zone = colorTemp / zoneSize;

  /* Clamp to last zone */
  if (zone >= NUM_COLOR_CHANNELS - 1) {
    zone = NUM_COLOR_CHANNELS - 2;
  }

  /* Position within the zone (0-65535) */
  posInZone = colorTemp - (zone * zoneSize);

  /* Linear interpolation between warm (zone) and cold (zone+1).
   * warm = (zoneSize - posInZone) / zoneSize
   * cold = posInZone / zoneSize
   * Scale by dimmer and use 32-bit to avoid overflow. */
  warm = ((unsigned long)(zoneSize - posInZone) * dimmer) / zoneSize;
  cold = ((unsigned long)posInZone * dimmer) / zoneSize;

  pwm->channel[zone] = (unsigned short)warm;
  pwm->channel[zone + 1] = (unsigned short)cold;
#endif
}
