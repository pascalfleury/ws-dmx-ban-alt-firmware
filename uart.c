#include "uart.h"
#include <mcs51/lint.h>
#include <mcs51/8051.h>
#include "board.h"
#include "stc8h.h"
#include "config.h"

volatile unsigned char busy; //true if we are currently sending a byte
volatile unsigned char dmxData[NUM_ADRESSES];
unsigned short dmxAddr = 0; //is written to from outside

/* DMA receive buffer in xdata.
 * We receive up to 512 data bytes after the start code.
 * The DMA will write received bytes here, then we copy
 * the relevant slice into dmxData. */
static volatile __xdata unsigned char dmxDmaBuffer[512];

/* State machine for DMX reception:
 * WAIT_BREAK: waiting for a break (framing error, RB8=0)
 * WAIT_START_CODE: break seen, waiting for start code byte
 * DMA_ACTIVE: start code was valid, DMA is receiving data bytes
 */
#define DMX_WAIT_BREAK      0
#define DMX_WAIT_START_CODE  1
#define DMX_DMA_ACTIVE       2

static volatile unsigned char dmxState = DMX_WAIT_BREAK;

/* Start DMA reception for up to 512 bytes */
static void dmaStartReceive()
{
  P_SW2 |= EAXFR;

  /* Clear DMA status */
  DMA_UR1R_STA = 0x00;

  /* Set DMA buffer address */
  DMA_UR1R_RXAH = (unsigned short)dmxDmaBuffer >> 8;
  DMA_UR1R_RXAL = (unsigned short)dmxDmaBuffer & 0xFF;

  /* Set number of bytes to receive (AMT = count - 1) */
  /* We want up to 512 bytes, but AMT is 8-bit (max 255).
   * So we receive up to 256 bytes at a time (AMT=255).
   * This is enough as long as dmxAddr + NUM_ADRESSES <= 256,
   * which covers addresses 1-246 with 10 channels. */
  if(dmxAddr + NUM_ADRESSES <= 256) {
    DMA_UR1R_AMT = (unsigned char)(dmxAddr + NUM_ADRESSES - 1);
  } else {
    DMA_UR1R_AMT = 255;
  }

  DMA_UR1R_DONE = 0x00;

  /* Enable DMA: bit 7 = enable, bit 6 = clear FIFO,
   * bit 0 = trigger (start) */
  DMA_UR1R_CR = 0xC1;

  /* Enable DMA interrupt (bit 0 = done interrupt enable) */
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

void uartInit()
{
  int i = 0;
  for(i = 0; i < NUM_ADRESSES; ++i) {
    dmxData[i] = 0;
  }
  for(i = 0; i < 512; ++i) {
    dmxDmaBuffer[i] = 0;
  }

  busy = 0;
  dmxState = DMX_WAIT_BREAK;

  /* DMX uses one start bit, 2 stop bits and no parity.
   * Mode 3: 8-bit variable baud rate async,
   * one start bit, one stop bit, one programmable stop bit (TB8)
   * SM0=1, SM1=1, SM2=0, REN=1, TB8=1 */
  SCON = 0xD8;

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
         * Start DMA to collect the data bytes. */
        dmxState = DMX_DMA_ACTIVE;
        dmaStartReceive();
      } else {
        /* Invalid start code or another break, reset */
        dmxState = DMX_WAIT_BREAK;
      }
    } else if(dmxState == DMX_DMA_ACTIVE) {
      /* If we get a UART interrupt while DMA is active,
       * it means we got a new break (RB8=0).
       * Stop DMA and process whatever we got. */
      if(RB8 == 0) {
        dmaStopReceive();
        dmxState = DMX_WAIT_START_CODE;
      }
      /* else: shouldn't happen, DMA handles data bytes */
    }
  }

  if(TI) {
    TI = 0;
    busy = 0;
  }
}

/* DMA UART1 receive completion interrupt.
 * Fires when DMA has received the requested number of bytes. */
void dmaUart1RInterrupt() __interrupt(DMA_UR1R_VECTOR) __using(1)
{
  unsigned char i;

  P_SW2 |= EAXFR;
  DMA_UR1R_STA = 0x00; //clear DMA status
  DMA_UR1R_CR = 0x00;  //disable DMA
  P_SW2 &= ~EAXFR;

  /* Copy relevant bytes from DMA buffer into dmxData.
   * DMA buffer index 0 = DMX channel 1 (byte after start code).
   * Our address range starts at dmxAddr (1-based),
   * so buffer index = dmxAddr - 1. */
  if(dmxAddr >= 1) {
    unsigned short bufStart = dmxAddr - 1;
    for(i = 0; i < NUM_ADRESSES; i++) {
      dmxData[i] = dmxDmaBuffer[bufStart + i];
    }
  }

  /* Signal DMX frame received by turning off power LED.
   * Main loop turns it back on, creating flicker when DMX is present. */
  PWR_LED = 1;

  dmxState = DMX_WAIT_BREAK;
}

void uartSendByte(unsigned char dat)
{
  while(busy);
  ACC = dat;
  busy = 1;
  SBUF = ACC;
}

void uartSendString(char *s)
{
  while(*s) {
    uartSendByte(*s++);
  }
}
