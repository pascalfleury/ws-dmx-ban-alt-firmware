#ifndef PWM_H
#define PWM_H

/* Initialize hardware PWM with 1200 steps at 24MHz */
void pwmInit(void);

/* Set PWM duty cycle for channels 1-3 (0-1199) */
void pwmSetChannel1(unsigned short duty);
void pwmSetChannel2(unsigned short duty);
void pwmSetChannel3(unsigned short duty);

#endif
