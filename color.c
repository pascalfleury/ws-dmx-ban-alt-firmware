#include "color.h"
#include "numeric.h"

/* Color temperature mapping for 3-channel boards.
 *
 * The 3 PWM channels drive LEDs of different white temperatures:
 *   PWM_1 = warm white
 *   PWM_2 = neutral white
 *   PWM_3 = cold white
 *
 * colorTemp range 0x0000..0xFFFF is split into 2 zones:
 *   Zone 0 (0x0000..0x7FFF): warm -> neutral crossfade
 *   Zone 1 (0x8000..0xFFFF): neutral -> cold crossfade
 *
 * Within each zone, we linearly interpolate between the two
 * adjacent channels. The result is then scaled by the dimmer value.
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
  unsigned short warm = 0;
  unsigned short neutral = 0;
  unsigned short cold = 0;
  unsigned short colorTemp = state->colorTemp;
  unsigned short dimmer = state->dimmer;
  unsigned short fade;

  if (colorTemp < 0x8000) {
    /* Zone 0: warm to neutral crossfade.
     * fade = 0 at colorTemp=0x0000, fade = 0xFFFF at colorTemp=0x7FFF */
    fade = colorTemp << 1;
    warm = 0xFFFF - fade;
    neutral = fade;
    cold = 0;
  } else {
    /* Zone 1: neutral to cold crossfade.
     * fade = 0 at colorTemp=0x8000, fade = 0xFFFF at colorTemp=0xFFFF */
    fade = (colorTemp - 0x8000) << 1;
    warm = 0;
    neutral = 0xFFFF - fade;
    cold = fade;
  }

  /* Scale by dimmer */
  pwm->channel[0] = scale16(warm, dimmer);
  pwm->channel[1] = scale16(neutral, dimmer);

#if NUM_PWM_CHANNELS >= 3
  pwm->channel[2] = scale16(cold, dimmer);
#endif
}
