#ifndef LED_H
#define LED_H

/* Initialize the DMX LED (off). */
void ledInit(void);

/* Call when a valid DMX frame is received.
 * Toggles the LED to create visible flicker. */
void ledOnFrame(void);

/* Update the DMX LED state. Call once per main loop iteration.
 * hasError: 1 = flash error pattern, 0 = normal operation. */
void ledUpdate(unsigned char hasError);

#endif /* LED_H */
