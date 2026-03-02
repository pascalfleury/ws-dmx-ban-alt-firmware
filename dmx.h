#ifndef DMX_H
#define DMX_H

typedef enum {
  DMX_MODE_SIMPLE = 0, /* 4 channels */
  DMX_MODE_FULL   = 1  /* 6 channels */
} dmx_mode_t;

#define DMX_SIMPLE_NUM_CHANNELS 4
#define DMX_FULL_NUM_CHANNELS   6

/* Decoded DMX state.
 * Coarse/fine pairs are combined into 16-bit values.
 * In simple mode, coarse values are shifted left by 8
 * to fill the full 16-bit range.
 *
 * Fields:
 *   dimmer      - general dimmer (0-65535)
 *   colorTemp   - color temperature (0-65535)
 *   strobeMode  - strobe mode (0=off, 1=flashing, 2=lightning)
 *   strobeSpeed - strobe speed (0=slow, 255=fast)
 *   mode        - current DMX mode (simple or full)
 *   address     - current DMX base address (1-based, clamped)
 */
typedef struct {
  dmx_mode_t mode;            /* current mode */
  unsigned short address;     /* current base address (raw from DIP) */

  unsigned short dimmer;      /* general dimmer (coarse + fine) */
  unsigned short colorTemp;   /* color temperature (coarse + fine) */
  unsigned char strobeMode;   /* strobe mode */
  unsigned char strobeSpeed;  /* strobe speed */
} DmxState;

/* Initialize a DmxState struct to sensible defaults.
 * Sets mode and address from DIP switches, and all
 * channel values to safe defaults (off, no strobe). */
void dmxInit(DmxState *state);

/* Returns 1 if the current DIP switch address and mode fit within the
 * 512-channel DMX universe, 0 otherwise.
 * Reads mode and address directly from DIP switches. */
unsigned char dmxAddressValid(void);

/* Checks for a new DMX frame and if present, decodes the
 * relevant channels into the provided state based on the current mode.
 * Always updates mode and address from DIP switches.
 * Returns 1 if channel data was updated, 0 if no new frame.
 * Only call this when dmxAddressValid() returns 1. */
unsigned char dmxUpdate(DmxState *state);

#endif
