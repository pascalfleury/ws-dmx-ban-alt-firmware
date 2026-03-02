#define FOSC  24000000L //System frequency.
#define BAUD  250000L    //UART1 baud-rate

/* Select the active board (uncomment one) */
#define BOARD_MINI_DMX_3CH_V1_2
//#define BOARD_WS_DMX_3CH_BAN_REV_4_6

/* Number of LED channels used for color temperature mixing.
 * Must not exceed NUM_PWM_CHANNELS defined by the board. */
#define NUM_LED_CHANNELS 3

/* Color temperatures of the LEDs on each channel (in Kelvin).
 * Must be listed in ascending order (warmest first).
 * Array length must equal NUM_LED_CHANNELS. */
#define LED_COLOR_TEMPERATURES { 2700, 4000, 6500 }
