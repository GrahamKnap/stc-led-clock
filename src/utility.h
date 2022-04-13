
#ifndef _UTILITY_H
#define _UTILITY_H

#include <stdint.h>

uint8_t MapBrightness(uint8_t adcValue);
uint8_t DecodeTemperature(uint8_t adcValue);

uint8_t BcdToDec(uint8_t bcd);
uint8_t DecToBcd(uint8_t dec);

uint8_t IncrementBcd(uint8_t value);
uint8_t IncrementHours(uint8_t h);
uint8_t IncrementMinutes(uint8_t m);
uint8_t ConvertHours(uint8_t h);
uint8_t DaysInMonth(uint8_t m);

#endif
