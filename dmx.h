#ifndef DMX_H
#define DMX_H

#include "config.h"

typedef enum {
  DMX_MODE_SIMPLE = 0, /* 4 channels */
  DMX_MODE_FULL   = 1  /* 6 channels */
} dmx_mode_t;

#define DMX_SIMPLE_NUM_CHANNELS 4
#define DMX_FULL_NUM_CHANNELS   6

/* DMX data buffer, filled by UART DMA and used by application */
extern volatile unsigned char dmxData[NUM_ADRESSES];

/* Returns the current DMX mode based on the function DIP switch */
dmx_mode_t dmxGetMode(void);

/* Returns the current DMX base address (1-based, clamped) */
unsigned short dmxGetAddress(void);

/* Reads DIP switches, updates address/mode, and tells UART layer
 * the current DMX range. Call this regularly from the main loop. */
void dmxUpdate(void);

#endif
