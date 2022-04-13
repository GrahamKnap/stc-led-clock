// based on example code in the STC15 datasheet

#include "adc.h"
#include "stc15.h"

// definitions for ADC_CONTR register
#define ADC_POWER   0x80        // ADC power control bit
#define ADC_SPEEDHH 0x60        // 90 clocks
#define ADC_SPEEDH  0x40        // 180 clocks
#define ADC_SPEEDL  0x20        // 360 clocks
#define ADC_SPEEDLL 0x00        // 540 clocks
#define ADC_FLAG    0x10        // ADC complete flag
#define ADC_START   0x08        // ADC start control bit
#define ADC_CHS(c)  (c)         // CHS2:0

// global variables
__bit adcBusy;
uint8_t adcResult;

void InitADC(uint8_t channel)
{
    // set the pin to open-drain mode
    P1M0 |= (1 << channel);
    P1M1 |= (1 << channel);

    // enable ADC function
    P1ASF |= (1 << channel);

    // enable ADC power, low-speed conversion
    ADC_CONTR = ADC_POWER | ADC_SPEEDL;

    // enable ADC interrupt
    EADC = 1;
}

void StartADC(uint8_t channel)
{
    while (adcBusy)
    {
        // wait for the previous conversion to complete
    }

    adcBusy = 1;
    ADC_CONTR = ADC_POWER | ADC_SPEEDL | ADC_START | ADC_CHS(channel);
}

void ISR_ADC() __interrupt(5) __using(1)
{
    ADC_CONTR &= ~(ADC_FLAG);
    adcResult = ADC_RES;
    adcBusy = 0;
}
