
#ifndef UART_H
#define UART_H

#include <mcs51/8051.h>
#include "stc8h.h"

void uartInit(void);

/* Returns 1 if a new complete DMX frame has been received since
 * the last call to uartClearFrameFlag(). */
unsigned char uartHasNewFrame(void);

/* Clears the new-frame flag. */
void uartClearFrameFlag(void);

/* Copy @p len bytes from the DMA receive buffer starting at
 * @p offset (0-based, where 0 = DMX channel 1) into @p dest.
 * Returns 1 on success, 0 if offset + len would exceed the buffer. */
unsigned char uartGetData(unsigned char *dest, unsigned short offset, unsigned char len);

#endif /* UART_H */
