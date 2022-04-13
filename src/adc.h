#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>

void ISR_ADC() __interrupt(5) __using(1);
void InitADC(uint8_t channel);
void StartADC(uint8_t channel);

extern __bit adcBusy;
extern uint8_t adcResult;

#endif // file
