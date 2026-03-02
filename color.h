#ifndef COLOR_H
#define COLOR_H

#include "dmx.h"
#include "pwm.h"

/* Compute the PWM output levels for the given DMX state.
 * Maps dimmer and colorTemp to the correct mix of warm/cold
 * white LEDs across the available PWM channels.
 *
 * Only the first NUM_LED_CHANNELS PWM channels are used for
 * color mixing. Any remaining PWM channels are set to 0.
 *
 * state: current DMX state (dimmer + colorTemp)
 * pwm:   output PWM state to be filled in
 */
void colorCompute(const DmxState *state, PwmState *pwm);

#endif
