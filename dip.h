
#ifndef DIP_H
#define DIP_H

void dipInit();
unsigned short readDmxAddr();
unsigned char readFunctionDip();

/* Reads DIP switches, clamps DMX address.
 * Requires dmxAddr (from uart.c) and NUM_ADRESSES (from config.h). */
void readDipSwitch();

#endif
