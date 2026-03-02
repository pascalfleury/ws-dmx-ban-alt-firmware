#include "uart.h"
#include <mcs51/lint.h>
#include <mcs51/8051.h>
#include "board.h"
#include "stc8h.h"
#include "config.h"

/* DMA receive buffer in xdata.
 * Index 0 = DMX channel 1 (first byte after start code).
 * Always receives full 512 bytes (or until next break). */
static __xdata volatile unsigned char dmxDmaBuffer[512];

static volatile unsigned char newFrameFlag = 0;

/* State machine for DMX reception:
 * WAIT_BREAK: waiting for a break (framing error, RB8=0)
 * WAIT_START_CODE: break seen, waiting for start code byte
 * DMA_FIRST_HALF: DMA is receiving first 256 bytes
 * DMA_SECOND_HALF: DMA is receiving second 256 bytes
 */
#define DMX_WAIT_BREAK       0
#define DMX_WAIT_START_CODE  1
#define DMX_DMA_FIRST_HALF   2
#define DMX_DMA_SECOND_HALF  3

static volatile unsigned char dmxState = DMX_WAIT_BREAK;

/* Start DMA reception for first 256 bytes */
static void dmaStartReceive()
{
  P_SW2 |= EAXFR;

  /* Clear DMA status */
  DMA_UR1R_STA = 0x00;

  /* Set DMA buffer address (start of buffer) */
  DMA_UR1R_RXAH = (unsigned short)dmxDmaBuffer >> 8;
  DMA_UR1R_RXAL = (unsigned short)dmxDmaBuffer & 0xFF;

  /* AMT is 8-bit, so max is 255 (= 256 bytes).
   * Receive first 256 bytes. */
  DMA_UR1R_AMT = 255;

  DMA_UR1R_DONE = 0x00;

  /* Enable DMA: bit 7 = enable, bit 6 = clear FIFO,
   * bit 0 = trigger (start) */
  DMA_UR1R_CR = 0xC1;

  /* Enable DMA interrupt (bit 0 = done interrupt enable) */
  DMA_UR1R_CFG = 0x01;

  P_SW2 &= ~EAXFR;
}

/* Start DMA reception for the second 256 bytes */
static void dmaStartReceiveSecondHalf()
{
  P_SW2 |= EAXFR;

  DMA_UR1R_STA = 0x00;

  /* Point to second half of buffer (offset 256) */
  DMA_UR1R_RXAH = (unsigned short)(dmxDmaBuffer + 256) >> 8;
  DMA_UR1R_RXAL = (unsigned short)(dmxDmaBuffer + 256) & 0xFF;

  DMA_UR1R_AMT = 255;
  DMA_UR1R_DONE = 0x00;

  DMA_UR1R_CR = 0xC1;
  DMA_UR1R_CFG = 0x01;

  P_SW2 &= ~EAXFR;
}

/* Stop DMA reception */
static void dmaStopReceive()
{
  P_SW2 |= EAXFR;
  DMA_UR1R_CR = 0x00;
  DMA_UR1R_STA = 0x00;
  DMA_UR1R_CFG = 0x00;
  P_SW2 &= ~EAXFR;
}

unsigned char uartHasNewFrame(void)
{
  return newFrameFlag;
}

void uartClearFrameFlag(void)
{
  newFrameFlag = 0;
}

unsigned char uartGetDmxData(unsigned char *dest, unsigned short offset, unsigned char len)
{
  unsigned char i;
  if (offset + len > 512) {
    return 0;
  }
  for (i = 0; i < len; i++) {
    dest[i] = dmxDmaBuffer[offset + i];
  }
  return 1;
}

void uartInit()
{
  int i = 0;
  for(i = 0; i < 512; ++i) {
    dmxDmaBuffer[i] = 0;
  }

  dmxState = DMX_WAIT_BREAK;

  /* DMX uses one start bit, 2 stop bits and no parity.
   * Mode 3: 8-bit variable baud rate async,
   * one start bit, one stop bit, one programmable stop bit (TB8)
   * SM0=1, SM1=1, SM2=0, REN=1, TB8=0 (no TX) */
  SCON = 0xD0;

  /* Calculate timer overflow values for BAUD rate */
  TL1 = (unsigned char)(65536 - (FOSC / 4 / BAUD));
  TH1 = (unsigned char)((65536 - (FOSC / 4 / BAUD)) >> 8);
  AUXR = 0x40; //T1 in 1T mode, use T1 as uart1 baud-rate generator

  TR1 = 1; //start timer 1

  PS = 0; //set uart interrupt to low priority

  ES = 1; //enable UART1 interrupt
  EA = 1; //enable all interrupts
}

void uartInterrupt() __interrupt(SI0_VECTOR) __using(1)
{
  /* The UART interrupt now only handles:
   * 1. Break detection (RB8=0 => framing error => DMX break)
   * 2. Start code validation
   * 3. TX completion
   * Data reception after start code is handled by DMA. */

  if(RI) {
    unsigned char dat = SBUF;
    RI = 0;

    if(dmxState == DMX_WAIT_BREAK) {
      /* We are waiting for a break.
       * A break shows up as RB8=0 (framing error). */
      if(RB8 == 0) {
        dmxState = DMX_WAIT_START_CODE;
      }
      /* else: ignore valid bytes while waiting for break */
    } else if(dmxState == DMX_WAIT_START_CODE) {
      if(RB8 == 1 && dat == 0) {
        /* Valid start code (0x00) received.
         * Start DMA to collect the first 256 data bytes. */
        dmxState = DMX_DMA_FIRST_HALF;
        dmaStartReceive();
      } else {
        /* Invalid start code or another break, reset */
        dmxState = DMX_WAIT_BREAK;
      }
    } else if(dmxState == DMX_DMA_FIRST_HALF || dmxState == DMX_DMA_SECOND_HALF) {
      /* If we get a UART interrupt while DMA is active,
       * it means we got a new break (RB8=0).
       * Stop DMA and signal frame complete. */
      if(RB8 == 0) {
        dmaStopReceive();
        newFrameFlag = 1;
        PWR_LED = 1;
        dmxState = DMX_WAIT_START_CODE;
      }
      /* else: shouldn't happen, DMA handles data bytes */
    }
  }

}

/* DMA UART1 receive completion interrupt.
 * Fires when DMA has received the requested number of bytes. */
void dmaUart1RInterrupt() __interrupt(DMA_UR1R_VECTOR) __using(1)
{
  P_SW2 |= EAXFR;
  DMA_UR1R_STA = 0x00; //clear DMA status
  DMA_UR1R_CR = 0x00;  //disable DMA
  P_SW2 &= ~EAXFR;

  if(dmxState == DMX_DMA_FIRST_HALF) {
    /* First 256 bytes received, start second half */
    dmxState = DMX_DMA_SECOND_HALF;
    dmaStartReceiveSecondHalf();
  } else {
    /* All 512 bytes received */
    newFrameFlag = 1;
    PWR_LED = 1;
    dmxState = DMX_WAIT_BREAK;
  }
}
