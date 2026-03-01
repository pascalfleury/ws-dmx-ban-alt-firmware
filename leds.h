#ifndef LEDS_H
#include <mcs51/lint.h>

//ATTETION there is an unrolled loop in leds.c depending on this number.
//         if you change this number, also fix the loop
#define NUM_LEDS 8

void ledInit();

#define LEDS_H
#endif
