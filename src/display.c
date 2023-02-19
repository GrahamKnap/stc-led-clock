// project definitions
#include "global.h"

// definitions for this file
#include "display.h"

// other local headers
#include "config.h"
#include "ds1302.h"
#include "timer.h"
#include "utility.h"

// system headers
#include "stc15.h"

// Global variables
uint8_t digit[4];
uint8_t brightLevel = DISPLAY_TICKS / 2;

// Local variables
static uint8_t displayDigit[4];

// The following definitions must be provided by global.h:
//
// #define COMMON_ANODE         // if applicable, else common cathode
// #define LED_COMMON_PORT      P3
// LED digit common connections, either common anodes or cathodes.
//
// #define LED_COMMON_MASK      0b00111100
// A value with bits set corresponding to LED common connections.
//
// #define LED_COMMON_BIT(pos)  (2 + (pos))
// Maps a digit position (0-3) to an output bit (0-7).
//
// #define LED_SEGMENT_PORT     P2
// The code assumes that Px.0 - Px.7 map to LED segments A - G, DP.
// See the SEVENSEG and DP_MASK macros in "display.h".

#define CONCAT_NX(a, b)     a ## b
#define CONCAT(a, b)        CONCAT_NX(a, b)

#define LED_SEGMENT_PXM0    CONCAT(LED_SEGMENT_PORT, M0)
#define LED_SEGMENT_PXM1    CONCAT(LED_SEGMENT_PORT, M1)
#define LED_COMMON_PXM0     CONCAT(LED_COMMON_PORT, M0)
#define LED_COMMON_PXM1     CONCAT(LED_COMMON_PORT, M1)

// Accesses to the LED Common register must be implemented so as to avoid
// affecting other pins within the same port. The following are OK:
//
// orl _P3, #0x04
// anl _P3, #0xC3
// setb _P3_2
// clr _P3_2
//
// The following can interfere with other functions:
//
// mov r6, _P3
// orl r6, a
// mov _P3, r6
//
// This was found to break a serial port assigned to P3.6/P3.7.

void InitDisplay(void)
{
    // PxM1 = 0, PxM0 = 0: 20mA sink, weak pull-up, quasi-bidir (default)
    // PxM1 = 0, PxM0 = 1: 20mA push/pull
    // PxM1 = 1, PxM0 = 0: input only
    // PxM1 = 1, PxM0 = 1: open drain
    //
    // The weak pull-up of ~200uA is insufficient for anodes,
    // so set anode connections to 20mA push/pull.
#ifdef COMMON_ANODE
    LED_COMMON_PXM0 |= LED_COMMON_MASK;
    //LED_COMMON_PXM1 &= ~(LED_COMMON_MASK);
    //LED_SEGMENT_PXM0 = 0;
    //LED_SEGMENT_PXM1 = 0;
#else
    //LED_COMMON_PXM0 &= ~(LED_COMMON_MASK);
    //LED_COMMON_PXM1 &= ~(LED_COMMON_MASK);
    LED_SEGMENT_PXM0 = 0xff;
    //LED_SEGMENT_PXM1 = 0;
#endif

#ifdef COMMON_ANODE
    LED_COMMON_PORT &= ~(LED_COMMON_MASK);
#else
    LED_COMMON_PORT |= (LED_COMMON_MASK);
#endif
}

// This routine is called by the timer ISR at 50 us (20 KHz), so be quick!
// At 22.1 MHz, the timer interrupt period is about 1105 CPU clock cycles.
void DisplayUpdateISR(void)
{
    static uint8_t pos;             // cycles through digit positions 0 to 3
    static uint8_t tick;            // counts from 0 to DISPLAY_TICKS - 1

    if (tick == 0)
    {
#ifdef COMMON_ANODE
        LED_SEGMENT_PORT = ~displayDigit[pos];

        switch (pos)
        {
            case 0: LED_COMMON_PORT |= (1 << LED_COMMON_BIT(0)); break;
            case 1: LED_COMMON_PORT |= (1 << LED_COMMON_BIT(1)); break;
            case 2: LED_COMMON_PORT |= (1 << LED_COMMON_BIT(2)); break;
            default: LED_COMMON_PORT |= (1 << LED_COMMON_BIT(3)); break;
        }
#else
        LED_SEGMENT_PORT = displayDigit[pos];

        switch (pos)
        {
            case 0: LED_COMMON_PORT &= ~(1 << LED_COMMON_BIT(0)); break;
            case 1: LED_COMMON_PORT &= ~(1 << LED_COMMON_BIT(1)); break;
            case 2: LED_COMMON_PORT &= ~(1 << LED_COMMON_BIT(2)); break;
            default: LED_COMMON_PORT &= ~(1 << LED_COMMON_BIT(3)); break;
        }
#endif
    }
    else if (tick >= brightLevel)
    {
#ifdef COMMON_ANODE
        LED_COMMON_PORT &= ~(LED_COMMON_MASK);
#else
        LED_COMMON_PORT |= (LED_COMMON_MASK);
#endif
    }

    tick++;

    if (tick >= DISPLAY_TICKS)
    {
        tick = 0;

#ifdef COMMON_ANODE
        LED_COMMON_PORT &= ~(LED_COMMON_MASK);
#else
        LED_COMMON_PORT |= (LED_COMMON_MASK);
#endif

        pos = (pos + 1) & 3;
    }
}

static const uint8_t hexadecimalGlyph[] = {
    SEVENSEG_0, SEVENSEG_1, SEVENSEG_2, SEVENSEG_3,
    SEVENSEG_4, SEVENSEG_5, SEVENSEG_6, SEVENSEG_7,
    SEVENSEG_8, SEVENSEG_9, SEVENSEG_A, SEVENSEG_b,
    SEVENSEG_C, SEVENSEG_d, SEVENSEG_E, SEVENSEG_F
};

#ifdef DIGIT_2_FLIP
static const uint8_t hexadecimalGlyphFlip[] = {
    ROTATE_DIGIT(SEVENSEG_0), ROTATE_DIGIT(SEVENSEG_1),
    ROTATE_DIGIT(SEVENSEG_2), ROTATE_DIGIT(SEVENSEG_3),
    ROTATE_DIGIT(SEVENSEG_4), ROTATE_DIGIT(SEVENSEG_5),
    ROTATE_DIGIT(SEVENSEG_6), ROTATE_DIGIT(SEVENSEG_7),
    ROTATE_DIGIT(SEVENSEG_8), ROTATE_DIGIT(SEVENSEG_9),
    ROTATE_DIGIT(SEVENSEG_A), ROTATE_DIGIT(SEVENSEG_b),
    ROTATE_DIGIT(SEVENSEG_C), ROTATE_DIGIT(SEVENSEG_d),
    ROTATE_DIGIT(SEVENSEG_E), ROTATE_DIGIT(SEVENSEG_F),
};
#endif

#ifdef OPT_DAY_ALPHA
static const uint8_t dayText[] = {
    SEVENSEG_S, ORIENT_DIGIT2(SEVENSEG_u), SEVENSEG_n,
    SEVENSEG_M, ORIENT_DIGIT2(SEVENSEG_o), SEVENSEG_n,
    SEVENSEG_t, ORIENT_DIGIT2(SEVENSEG_U), SEVENSEG_E,
    SEVENSEG_W, ORIENT_DIGIT2(SEVENSEG_E), SEVENSEG_d,
    SEVENSEG_t, ORIENT_DIGIT2(SEVENSEG_h), SEVENSEG_u,
    SEVENSEG_F, ORIENT_DIGIT2(SEVENSEG_r), SEVENSEG_i,
    SEVENSEG_S, ORIENT_DIGIT2(SEVENSEG_A), SEVENSEG_t,
};

void DisplayDayOfWeek(void)
{
    // clock.weekday reads 1 through 7.
    // The DS1302 doesn't care how we map these to day names;
    // nor should the user, because we never display the number.
    uint8_t w = (clock.weekday - 1) * 3;
    digit[0] = 0;
    digit[1] = dayText[w++];
    digit[2] = dayText[w++];
    digit[3] = dayText[w];
}
#else // not OPT_DAY_ALPHA
void DisplayDayOfWeek(void)
{
    // clock.weekday reads 1 through 7.
    digit[0] = 0;
    digit[1] = SEVENSEG_dash;
#ifdef DIGIT_2_FLIP
    digit[2] = hexadecimalGlyphFlip[clock.weekday];
#else
    digit[2] = hexadecimalGlyph[clock.weekday];
#endif
    digit[3] = SEVENSEG_dash;
}
#endif // OPT_DAY_ALPHA

#ifdef OPT_MONTH_ALPHA
static const uint8_t monthText[] = {
    SEVENSEG_J, ROTATE_DIGIT(SEVENSEG_J), SEVENSEG_A,
    SEVENSEG_F, ROTATE_DIGIT(SEVENSEG_F), SEVENSEG_E,
    SEVENSEG_M, ROTATE_DIGIT(SEVENSEG_M), SEVENSEG_r,
    SEVENSEG_A, ROTATE_DIGIT(SEVENSEG_A), SEVENSEG_P,
    SEVENSEG_M, ROTATE_DIGIT(SEVENSEG_M), SEVENSEG_A,
    SEVENSEG_J, ROTATE_DIGIT(SEVENSEG_J), SEVENSEG_n,
    SEVENSEG_J, ROTATE_DIGIT(SEVENSEG_J), SEVENSEG_L,
    SEVENSEG_A, ROTATE_DIGIT(SEVENSEG_A), SEVENSEG_u,
    SEVENSEG_S, ROTATE_DIGIT(SEVENSEG_S), SEVENSEG_E,
    SEVENSEG_O, ROTATE_DIGIT(SEVENSEG_O), SEVENSEG_c,
    SEVENSEG_n, ROTATE_DIGIT(SEVENSEG_n), SEVENSEG_o,
    SEVENSEG_d, ROTATE_DIGIT(SEVENSEG_d), SEVENSEG_E,
};

void DisplayMonth(uint8_t m)
{
    // clock.month reads 0x01 to 0x12 (BCD).
    // Map months 0x01 - 0x09 to 0-8.
    // Map months 0x10 - 0x12 to 9-11.
    m -= (m < 0x10) ? 1 : 7;
    m *= 3;

    if (monthFirst)
    {
        digit[0] = monthText[m];
        digit[1] = monthText[m + 2];
    }
    else
    {
#ifdef DIGIT_2_FLIP
        digit[2] = monthText[m + 1];
#else
        digit[2] = monthText[m];
#endif
        digit[3] = monthText[m + 2];
    }
}
#else // not OPT_MONTH_ALPHA
void DisplayMonth(uint8_t m)
{
    if (monthFirst)
    {
        DisplayHexLeft(m);
    }
    else
    {
        DisplayHexRight(m);
    }
}
#endif // OPT_MONTH_ALPHA

void DisplayHexLeft(uint8_t value)
{
    digit[0] = hexadecimalGlyph[value >> 4];
    digit[1] = hexadecimalGlyph[value & 0x0f];
}

void DisplayHexRight(uint8_t value)
{
#ifdef DIGIT_2_FLIP
    digit[2] = hexadecimalGlyphFlip[value >> 4];
#else
    digit[2] = hexadecimalGlyph[value >> 4];
#endif

    digit[3] = hexadecimalGlyph[value & 0x0f];
}

void DisplayHours(uint8_t h)
{
    h = ConvertHours(h);

#ifdef OPT_BLANK_ZERO
    if ((h >> 4) != 0) digit[0] = hexadecimalGlyph[h >> 4];
    digit[1] = hexadecimalGlyph[h & 0x0f];
#else
    DisplayHexLeft(h);
#endif
}

void DisplayTemperature(uint8_t t)
{
    DisplayHexLeft(t);
#ifdef DIGIT_2_FLIP
    digit[2] = ((degreesF) ? ROTATE_DIGIT(SEVENSEG_F) : ROTATE_DIGIT(SEVENSEG_C)) | DP_BIT;
#else
    digit[2] = (degreesF) ? SEVENSEG_F : SEVENSEG_C;
#endif
    digit[3] = 0;
}

void DisplayColon(void)
{
    digit[1] |= DP_BIT;

#ifdef DIGIT_2_FLIP
    digit[2] |= DP_BIT;
#endif
}

void DisplayPM(uint8_t hours)
{
    if (twelveHour && hours > 0x11)
    {
        digit[3] |= DP_BIT;
    }
}

void CommitDisplay(void)
{
    displayDigit[0] = digit[0];
    displayDigit[1] = digit[1];
    displayDigit[2] = digit[2];
    displayDigit[3] = digit[3];
}
