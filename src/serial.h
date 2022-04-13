#ifndef _SERIAL_H
#define _SERIAL_H

#include "global.h"

void InitSerial(void);

#ifdef DEBUG

#include <stdint.h>

// SDCC requires that "a prototype of the ISR must be present or included in
// the file that contains the function 'main'"
void ISR_UART1(void) __interrupt 4 __using 1;

void PrintChar(uint8_t c);
void PrintHex(uint8_t value);
void PrintString(const char * s);

#endif // DEBUG

#endif // _SERIAL_H
