#ifndef BOARD_H
#define BOARD_H

/* Select the active board here */
#include "boards/mini_dmx_3ch_v1_2.h"

#endif
#ifndef BOARD_H
#define BOARD_H

#include "config.h"

#if defined(BOARD_MINI_DMX_3CH_V1_2)
  #include "boards/mini_dmx_3ch_v1_2.h"
#elif defined(BOARD_WS_DMX_3CH_BAN_REV_4_6)
  #include "boards/ws_dmx_3ch_ban_rev_4_6.h"
#else
  #error "No board selected. Define a BOARD_* constant in config.h"
#endif

#endif
#ifndef BOARD_H
#define BOARD_H

#include "config.h"

#if defined(BOARD_MINI_DMX_3CH_V1_2)
  #include "boards/mini_dmx_3ch_v1_2.h"
#elif defined(BOARD_WS_DMX_3CH_BAN_REV_4_6)
  #include "boards/ws_dmx_3ch_ban_rev_4_6.h"
#else
  #error "No board selected. Define a BOARD_* constant in config.h"
#endif

#if NUM_COLOR_CHANNELS > NUM_PWM_CHANNELS
  #error "NUM_COLOR_CHANNELS exceeds NUM_PWM_CHANNELS for the selected board"
#endif

#endif
