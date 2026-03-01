
#ifndef UART_H
#define UART_H

void uartInterrupt();

void dmaUart1RInterrupt();

void uartInit();

void uartSendByte(unsigned char dat);

void uartSendString(char *s);


#endif
