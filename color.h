#ifndef COLOR_H
#define COLOR_H

#include "dmx.h"
#include "pwm.h"

/* Compute the PWM output levels for the given DMX state.
 * Maps dimmer and colorTemp to the correct mix of warm/cold
 * white LEDs across the available PWM channels.
 *
 * state: current DMX state (dimmer + colorTemp)
 * pwm:   output PWM state to be filled in
 */
void colorCompute(const DmxState *state, PwmState *pwm);

#endif
#ifndef COLOR_H
#define COLOR_H

#include "dmx.h"
#include "pwm.h"

/* Compute PWM channel values from DMX state.
 * Takes the dimmer and color temperature from dmxState,
 * and fills in the pwmState with the appropriate duty cycles
 * for each PWM channel to achieve the desired color temperature. */
void colorCompute(const DmxState *dmxState, PwmState *pwmState);

#endif
