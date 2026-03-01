#include "pwm.h"
#include "board.h"
#include "stc8h.h"

/* PWM Configuration:
 * - 24MHz system clock
 * - 65536 steps resolution (full 16-bit range)
 * - Frequency = 24MHz / 65536 = ~366Hz
 * - Channels: P1.7 (PWM1/O1), P1.2 (PWM2/O2), P1.0 (PWM3/O3)
 */

void pwmInit(void) {
  /* Enable extended SFR access */
  P_SW2 |= EAXFR;
  
  /* Configure PWM period (ARR = 65535 for 65536 steps, 0-indexed) */
  PWMA_ARRH = 0xFF;
  PWMA_ARRL = 0xFF;
  
  /* No prescaler needed (PSC = 0 for 24MHz / 1 = 24MHz) */
  PWMA_PSCRH = 0;
  PWMA_PSCRL = 0;
  
  /* Configure channels 1-3 in PWM mode 1 (OCxM = 110b) */
  /* CCMR format: bit[6:4] = OCxM mode, bit[3] = OCxPE preload enable */
  PWMA_CCMR1 = 0x68;  /* PWM mode 1, preload enabled */
  PWMA_CCMR2 = 0x68;
  PWMA_CCMR3 = 0x68;
  
  /* Enable output channels (CCxE bits) */
  PWMA_CCER1 = 0x11;  /* Enable CH1 and CH2 outputs */
  PWMA_CCER2 = 0x01;  /* Enable CH3 output */
  
  /* Pin selection: map to P1.7, P1.2, P1.0 */
  /* PS register bits select alternate pin mappings */
  PWMA_PS = 0x00;  /* Default mapping */
  
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

void pwmSetChannel1(unsigned short duty) {
  P_SW2 |= EAXFR;
  PWMA_CCR1H = (duty >> 8);
  PWMA_CCR1L = (duty & 0xFF);
  P_SW2 &= ~EAXFR;
}

void pwmSetChannel2(unsigned short duty) {
  P_SW2 |= EAXFR;
  PWMA_CCR2H = (duty >> 8);
  PWMA_CCR2L = (duty & 0xFF);
  P_SW2 &= ~EAXFR;
}

void pwmSetChannel3(unsigned short duty) {
  P_SW2 |= EAXFR;
  PWMA_CCR3H = (duty >> 8);
  PWMA_CCR3L = (duty & 0xFF);
  P_SW2 &= ~EAXFR;
}
