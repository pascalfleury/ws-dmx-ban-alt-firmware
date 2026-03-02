#include "pwm.h"
#include "stc8h.h"

/* PWM Configuration:
 * - 24MHz system clock
 * - 65536 steps resolution (full 16-bit range)
 * - Frequency = 24MHz / 65536 = ~366Hz
 * - Channels mapped per board header
 */

void pwmInit(void)
{
  /* Enable extended SFR access */
  P_SW2 |= EAXFR;

  /* Configure PWM period (ARR = 65535 for 65536 steps, 0-indexed) */
  PWMA_ARRH = 0xFF;
  PWMA_ARRL = 0xFF;

  /* No prescaler needed (PSC = 0 for 24MHz / 1 = 24MHz) */
  PWMA_PSCRH = 0;
  PWMA_PSCRL = 0;

  /* Configure channels in PWM mode 1 (OCxM = 110b, preload enabled) */
  PWMA_CCMR1 = 0x68;
  PWMA_CCMR2 = 0x68;
  PWMA_CCMR3 = 0x68;

  /* Enable output channels (CCxE bits) */
  PWMA_CCER1 = 0x11;  /* Enable CH1 and CH2 outputs */
  PWMA_CCER2 = 0x01;  /* Enable CH3 output */

  /* Pin selection: default mapping */
  PWMA_PS = 0x00;

  /* Enable main output (MOE bit in BKR) */
  PWMA_BKR = 0x80;

  /* Enable PWM outputs */
  PWMA_ENO = 0x07;  /* Enable PWM1P, PWM2P, PWM3P */

  /* Initialize all channels to 0 */
  PWMA_CCR1H = 0;
  PWMA_CCR1L = 0;
  PWMA_CCR2H = 0;
  PWMA_CCR2L = 0;
  PWMA_CCR3H = 0;
  PWMA_CCR3L = 0;

  /* Start counter: CEN bit in CR1 */
  PWMA_CR1 = 0x01;

  /* Disable extended SFR access */
  P_SW2 &= ~EAXFR;
}

void pwmStateInit(PwmState *state)
{
  unsigned char i;
  for (i = 0; i < NUM_PWM_CHANNELS; i++) {
    state->channel[i] = 0;
  }
}

void pwmSetChannel(unsigned char ch, unsigned short duty)
{
  P_SW2 |= EAXFR;
  switch (ch) {
    case 0:
      PWMA_CCR1H = (duty >> 8);
      PWMA_CCR1L = (duty & 0xFF);
      break;
    case 1:
      PWMA_CCR2H = (duty >> 8);
      PWMA_CCR2L = (duty & 0xFF);
      break;
    case 2:
      PWMA_CCR3H = (duty >> 8);
      PWMA_CCR3L = (duty & 0xFF);
      break;
    default:
      break;
  }
  P_SW2 &= ~EAXFR;
}

void pwmUpdate(const PwmState *state)
{
  P_SW2 |= EAXFR;

  PWMA_CCR1H = (state->channel[0] >> 8);
  PWMA_CCR1L = (state->channel[0] & 0xFF);

  PWMA_CCR2H = (state->channel[1] >> 8);
  PWMA_CCR2L = (state->channel[1] & 0xFF);

#if NUM_PWM_CHANNELS >= 3
  PWMA_CCR3H = (state->channel[2] >> 8);
  PWMA_CCR3L = (state->channel[2] & 0xFF);
#endif

#if NUM_PWM_CHANNELS >= 4
  PWMA_CCR4H = (state->channel[3] >> 8);
  PWMA_CCR4L = (state->channel[3] & 0xFF);
#endif

  P_SW2 &= ~EAXFR;
}
