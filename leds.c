#include "leds.h"
#include "config.h"
#include "board.h"

//software pwm for leds

//brightness array for leds, this is written to from somewhere else
volatile unsigned char ledBrightness[NUM_LEDS];

//27136 interrupts/s @ 2007916.66 increments/s
//#define TIMER_START 65462

//37888 interrupts/s = ~148hz led update rate
//#define TIMER_START (65536-53) 

//54272 interrupts/s = ~212hz led update rate
//#define TIMER_START (65536-37) 

//80384 interrupts/s = ~314hz led update rate
//#define TIMER_START (65536-25) 

//105728 interrupts/s ( every 9,45823us) = ~413hz led update rate 
#define TIMER_START (65536-19) 

void ledsInit()
{
    LED_1 = 0;
    LED_2 = 0;
    LED_3 = 0;
    LED_4 = 0;
    LED_5 = 0;
    LED_6 = 0;
    LED_7 = 0;
    LED_8 = 0;

    P3M0 = LED_P3M0_MASK;
    P2M0 = LED_P2M0_MASK;

    AUXR &= ~0x80;   // Set timer0 clock source to sysclk/12 (12T mode)
    TMOD &= 0xF0;    // Clear 4bit field for timer0

    //set reload counter
    TH0 = TIMER_START >> 8;
    TL0 = (unsigned char)TIMER_START;

    // set timer 0 interrupt priority to high
    // this is done to ensure stable pwm.
    // if we set this to low the uart will interrupt
    // the pwm and cause visible flicker when the leds are
    // at low brightness 
    PT0 = 1; 

    ET0 = 1; // Enable Timer0 interrupts
    EA  = 1; // Global interrupt enable

    TR0 = 1; // Start Timer 0
}


volatile unsigned char timer0Cnt = 0;
void timer0Interrupt()  __interrupt(1) __using(1)
{

    //FIXME this code is not generic at all because there seems to be no way
    //      to put __sbit into an array? wtf?

    //NOTE currently 255 is not fully on, it will still turn of for one cycle.
    // FIXME try to fix it without decreasing the performance
    if(timer0Cnt < ledBrightness[0])
    {
        LED_1 = 1;
    }
    else
    {
        LED_1 = 0;
    }

    if(timer0Cnt < ledBrightness[1])
    {
        LED_2 = 1;
    }
    else
    {
        LED_2 = 0;
    }

    if(timer0Cnt < ledBrightness[2])
    {
        LED_3 = 1;
    }
    else
    {
        LED_3 = 0;
    }

    if(timer0Cnt < ledBrightness[3])
    {
        LED_4 = 1;
    }
    else
    {
        LED_4 = 0;
    }

    if(timer0Cnt < ledBrightness[4])
    {
        LED_5 = 1;
    }
    else
    {
        LED_5 = 0;
    }

    if(timer0Cnt < ledBrightness[5])
    {
        LED_6 = 1;
    }
    else
    {
        LED_6 = 0;
    }

    if(timer0Cnt < ledBrightness[6])
    {
        LED_7 = 1;
    }
    else
    {
        LED_7 = 0;
    }

    if(timer0Cnt < ledBrightness[7])
    {
        LED_8 = 1;
    }
    else
    {
        LED_8 = 0;
    }

    timer0Cnt++;

}
