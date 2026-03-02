#ifndef BOARD_MINI_DMX_3CH_V1_2_H
#define BOARD_MINI_DMX_3CH_V1_2_H

#include <mcs51/8051.h>
#include "../stc15w.h"

/* Board: MINI-DMX-3CH-V1.2
 * Maps physical ports to logical names.
 */

/* Number of hardware PWM channels on this board */
#define NUM_PWM_CHANNELS 3

/* PWM outputs */
#define PWM_1       P1_4
#define PWM_2       P1_2
#define PWM_3       P1_0

/* UART (directly on P3.0/P3.1, no remapping needed) */
#define PIN_RXD     P3_0
#define PIN_TXD     P3_1

/* DMX LED */
#define DMX_LED     P3_2

/* DIP switches (address) */
#define DIP_1       P3_3
#define DIP_2       P3_4
#define DIP_3       P3_5
#define DIP_4       P3_6
#define DIP_5       P3_7
#define DIP_6       P2_4
#define DIP_7       P2_5
#define DIP_8       P2_6
#define DIP_9       P2_7

/* DIP switch (mode) */
#define DIP_10_MODE P0_0

/* Power LED */
#define PWR_LED     P0_3

/* LED outputs (accent/indicator LEDs driven by software PWM) */
#define LED_1       P3_4
#define LED_2       P3_5
#define LED_3       P2_1
#define LED_4       P2_2
#define LED_5       P2_3
#define LED_6       P2_4
#define LED_7       P2_5
#define LED_8       P2_6

/* Port mode registers for LED outputs:
 * P3.4, P3.5 => bits 4,5 of P3M0 => 0x30
 * P2.1-P2.6  => bits 1-6 of P2M0 => 0x7E
 */
#define LED_P3M0_MASK 0x30
#define LED_P2M0_MASK 0x7E

#endif
