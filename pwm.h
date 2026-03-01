#ifndef PWM_H
#define PWM_H

/* Initialize hardware PWM with 65536 steps (full 16-bit range) at 24MHz */
void pwmInit(void);

/* Set PWM duty cycle for channels 1-3 (0-65535) */
void pwmSetChannel1(unsigned short duty);
void pwmSetChannel2(unsigned short duty);
void pwmSetChannel3(unsigned short duty);

#endif
