#include "dmx.h"
#include "dip.h"
#include "uart.h"
#include "numeric.h"

/* Number of DMX channels per mode, indexed by dmx_mode_t */
static const unsigned char dmxNumChannels[] = {
  DMX_SIMPLE_NUM_CHANNELS, /* DMX_MODE_SIMPLE */
  DMX_FULL_NUM_CHANNELS    /* DMX_MODE_FULL */
};

#define DMX_MAX_CHANNELS max(DMX_SIMPLE_NUM_CHANNELS, DMX_FULL_NUM_CHANNELS)

dmx_mode_t dmxGetMode(void)
{
  return readFunctionDip() ? DMX_MODE_FULL
                           : DMX_MODE_SIMPLE;
}

unsigned short dmxGetAddress(void)
{
  unsigned short addr = readDmxAddr();
  unsigned short maxAddr;
  unsigned char numChannels = dmxNumChannels[dmxGetMode()];

  /* Clamp address to valid range */
  if (addr == 0) {
    addr = 1;
  }

  maxAddr = 512 - numChannels;
  if (addr > maxAddr) {
    addr = maxAddr;
  }

  return addr;
}

/* Decode raw bytes for simple mode (4 channels):
 *   raw[0] = dimmer coarse
 *   raw[1] = color temperature coarse
 *   raw[2] = strobe mode
 *   raw[3] = strobe speed
 * Coarse values are shifted left by 8 to fill 16-bit range. */
static void decodeSimple(DmxState *state, unsigned char *raw)
{
  state->dimmer      = (unsigned short)raw[0] << 8;
  state->colorTemp   = (unsigned short)raw[1] << 8;
  state->strobeMode  = raw[2];
  state->strobeSpeed = raw[3];
}

/* Decode raw bytes for full mode (6 channels):
 *   raw[0] = dimmer coarse
 *   raw[1] = dimmer fine
 *   raw[2] = color temperature coarse
 *   raw[3] = color temperature fine
 *   raw[4] = strobe mode
 *   raw[5] = strobe speed
 * Coarse/fine pairs are combined: (coarse << 8) | fine. */
static void decodeFull(DmxState *state, unsigned char *raw)
{
  state->dimmer      = ((unsigned short)raw[0] << 8) | raw[1];
  state->colorTemp   = ((unsigned short)raw[2] << 8) | raw[3];
  state->strobeMode  = raw[4];
  state->strobeSpeed = raw[5];
}

void dmxInit(DmxState *state)
{
  state->mode        = dmxGetMode();
  state->address     = dmxGetAddress();
  state->dimmer      = 0;
  state->colorTemp   = 0;
  state->strobeMode  = 0;
  state->strobeSpeed = 0;
}

unsigned char dmxUpdate(DmxState *state)
{
  unsigned char numChannels;
  unsigned char raw[DMX_MAX_CHANNELS];

  state->mode = dmxGetMode();
  state->address = dmxGetAddress();

  numChannels = dmxNumChannels[state->mode];

  /* If a new frame has been received, decode it */
  if (uartHasNewFrame()) {
    uartClearFrameFlag();

    /* DMA buffer is 0-based (index 0 = DMX channel 1).
     * Our address is 1-based, so offset = address - 1. */
    if (uartGetDmxData(raw, state->address - 1, numChannels)) {
      if (state->mode == DMX_MODE_FULL) {
        decodeFull(state, raw);
      } else {
        decodeSimple(state, raw);
      }
      return 1;
    }
  }

  return 0;
}
