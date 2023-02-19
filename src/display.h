#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdint.h>

// This number dictates how many brightness levels are available.
// Smaller numbers produce faster refresh rates, or less flicker.
// refresh Hz = T0FREQ / (4 digits * DISPLAY_TICKS)
#define DISPLAY_TICKS   50

// Segments A through F are in clockwise order around the perimeter,
// starting with segment A at the top. Segment G is in the center.

#define SEVENSEG(a,b,c,d,e,f,g) \
    (uint8_t)((0 << 7) | ((g) << 6) | ((f) << 5) | ((e) << 4) \
    | ((d) << 3) | ((c) << 2) | ((b) << 1) | ((a) << 0))

#define SEVENSEG_blank 0

#define SEVENSEG_0 SEVENSEG(1,1,1,1,1,1,0)
#define SEVENSEG_1 SEVENSEG(0,1,1,0,0,0,0)
#define SEVENSEG_2 SEVENSEG(1,1,0,1,1,0,1)
#define SEVENSEG_3 SEVENSEG(1,1,1,1,0,0,1)
#define SEVENSEG_4 SEVENSEG(0,1,1,0,0,1,1)
#define SEVENSEG_5 SEVENSEG(1,0,1,1,0,1,1)
#define SEVENSEG_6 SEVENSEG(1,0,1,1,1,1,1)
#define SEVENSEG_7 SEVENSEG(1,1,1,0,0,0,0)
#define SEVENSEG_8 SEVENSEG(1,1,1,1,1,1,1)
#define SEVENSEG_9 SEVENSEG(1,1,1,1,0,1,1)

#define SEVENSEG_A SEVENSEG(1,1,1,0,1,1,1)
#define SEVENSEG_b SEVENSEG(0,0,1,1,1,1,1)
#define SEVENSEG_C SEVENSEG(1,0,0,1,1,1,0)
#define SEVENSEG_c SEVENSEG(0,0,0,1,1,0,1)
#define SEVENSEG_d SEVENSEG(0,1,1,1,1,0,1)
#define SEVENSEG_E SEVENSEG(1,0,0,1,1,1,1)
#define SEVENSEG_F SEVENSEG(1,0,0,0,1,1,1)
#define SEVENSEG_g SEVENSEG(1,1,1,1,0,1,1)
#define SEVENSEG_H SEVENSEG(0,1,1,0,1,1,1)
#define SEVENSEG_h SEVENSEG(0,0,1,0,1,1,1)
#define SEVENSEG_I SEVENSEG(0,1,1,0,0,0,0)
#define SEVENSEG_i SEVENSEG(0,0,1,0,0,0,0)
#define SEVENSEG_J SEVENSEG(0,1,1,1,1,0,0)
#define SEVENSEG_L SEVENSEG(0,0,0,1,1,1,0)
#define SEVENSEG_M SEVENSEG(1,0,1,0,1,0,0)  // ugly
#define SEVENSEG_n SEVENSEG(0,0,1,0,1,0,1)
#define SEVENSEG_O SEVENSEG(1,1,1,1,1,1,0)
#define SEVENSEG_o SEVENSEG(0,0,1,1,1,0,1)
#define SEVENSEG_P SEVENSEG(1,1,0,0,1,1,1)
#define SEVENSEG_q SEVENSEG(1,1,1,0,0,1,0)
#define SEVENSEG_r SEVENSEG(0,0,0,0,1,0,1)
#define SEVENSEG_S SEVENSEG(1,0,1,1,0,1,1)
#define SEVENSEG_t SEVENSEG(0,0,0,1,1,1,1)
#define SEVENSEG_U SEVENSEG(0,1,1,1,1,1,0)
#define SEVENSEG_u SEVENSEG(0,0,1,1,1,0,0)
#define SEVENSEG_W SEVENSEG(0,1,0,1,0,1,0)  // ugly
#define SEVENSEG_y SEVENSEG(0,1,1,1,0,1,1)

#define SEVENSEG_dash SEVENSEG(0,0,0,0,0,0,1)

// Inverting a digit exchanges segments A-C with D-F.
// The center segment G and decimal point are unaffected.

#define ROTATE_DIGIT(v) \
    (uint8_t)(((v) & 0xc0) | (((v) & 0x07) << 3) | (((v) & 0x38) >> 3))

#if DIGIT_2_FLIP
    #define ORIENT_DIGIT2(v) ROTATE_DIGIT(v)
#else
    #define ORIENT_DIGIT2(v) (v)
#endif

#define DP_BIT      0x80

extern uint8_t brightLevel;     // not BCD
extern uint8_t digit[4];

void InitDisplay(void);
void DisplayUpdateISR(void);
void CommitDisplay(void);
void DisplayHexLeft(uint8_t value);
void DisplayHexRight(uint8_t value);
void DisplayHours(uint8_t bcdHours);
void DisplayMonth(uint8_t bcdMonth);
void DisplayTemperature(uint8_t bcdTemperature);
void DisplayDayOfWeek(void);
void DisplayColon(void);
void DisplayPM(uint8_t bcdHours);

#endif
