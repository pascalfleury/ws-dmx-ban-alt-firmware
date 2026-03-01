
#ifndef UART_H
#define UART_H

#include <mcs51/8051.h>
#include "stc8h.h"

void uartInit();

void uartSendByte(unsigned char dat);

void uartSendString(char *s);


#endif
