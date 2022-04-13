#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

extern volatile uint8_t userTimer100;
extern volatile uint8_t clockTenths;

extern __bit pressedS1;         // true when pressed, user clears
extern __bit pressedS2;         // true when pressed, user clears

#ifdef S3
extern __bit pressedS3;         // true when pressed, user clears
#endif

extern __bit fastFlashState;
extern __bit flashStateS2;

// SDCC requires that "a prototype of the ISR must be present or included in
// the file that contains the function main"
void ISR_Timer0() __interrupt (1);

void InitTimer0(void);
__bit CheckResetPressed(void);

#endif
