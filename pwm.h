#ifndef PWM_H
#define PWM_H

#include "board.h"

/* PWM state: one 16-bit duty cycle per channel.
 * Values range from 0 (off) to 65535 (fully on). */
typedef struct {
  unsigned short channel[NUM_PWM_CHANNELS];
} PwmState;

/* Initialize hardware PWM with 65536 steps (full 16-bit range) at 24MHz */
void pwmInit(void);

/* Initialize a PwmState struct to all channels off */
void pwmStateInit(PwmState *state);

/* Apply the duty cycles in the PwmState to the hardware PWM outputs */
void pwmUpdate(const PwmState *state);

/* Set PWM duty cycle for an individual channel (0-based index, 0-65535) */
void pwmSetChannel(unsigned char ch, unsigned short duty);

#endif /* PWM_H */
