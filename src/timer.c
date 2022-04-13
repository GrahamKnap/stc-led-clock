// project definitions
#include "global.h"

// definitions for this file
#include "timer.h"

// other local headers
#include "chime.h"
#include "display.h"
#include "ds1302.h"

// system headers
#include "stc15.h"

// AUXR bits
#define T0DIV1  0x80  // T0x12
#define T1DIV1  0x40  // T1x12
#define S1DIV2  0x20  // UART_M0x6
#define T2RUN   0x10  // T2R
#define T2COUNT 0x08  // T2_C/nT
#define T2DIV1  0x04  // T2x12
#define NAUXRAM 0x02  // EXTRAM
#define S1ST2   0x01


#define T0FREQ          25000
#define TICKS_PER_MS    (T0FREQ / 1000)

#define SW_PRESS_TIME   40  // ms
#define SW_REPEAT_TIME  180 // ms


volatile uint8_t userTimer100;  // user delay in 100ms ticks (25.5 seconds max)
volatile uint8_t clockTenths;   // approximate tenths of a second

__bit pressedS1;          // true when pressed, user clears
__bit pressedS2;          // true when pressed, user clears

#ifdef S3
__bit pressedS3;          // true when pressed, user clears
#endif

__bit fastFlashState;

// used for detecting S2 auto-repeat to
// prevent constant flashing of display while changing
__bit flashStateS2;


void InitTimer0(void)
{
    AUXR |= T0DIV1; // The clock source of Timer 0 is SYSclk/1
    TH0 = (65536 - (FOSC / T0FREQ)) >> 8;
    TL0 = (65536 - (FOSC / T0FREQ)) & 0xFF;
    TF0 = 0;        // Clear overflow flag
    TR0 = 1;        // Timer0 start run
    PT0 = 1;        // high interrupt priority
    ET0 = 1;        // enable timer0 interrupt
    EA  = 1;        // global interrupt enable
}

static void UpdateClockTenths(void)
{
    // Approximately synchronize to the clock, when possible.
    // In most setup modes, the clock is not read/updated.
    // Synchronize only if the clock appears to be regular.
    //
    // For STC15 MCUs, the internal clock is accurate to +/- 0.3%
    // or 3000 ppm, or a maximum error of +/- 3 ms per second.
    // The 32.768 KHz clock crystal is likely accurate to ~100 ppm.
    static uint8_t milliseconds = 100;
    static uint8_t prevSeconds;
    static uint8_t syncTimer;

    if (prevSeconds != clock.second)
    {
        prevSeconds = clock.second;

        if (syncTimer == 9 || syncTimer == 10)
        {
            milliseconds = 100;
            clockTenths = 0;
        }

        syncTimer = 0;
    }

    // clock sync and tenths of a second
    milliseconds--;

    if (milliseconds == 0)
    {
        milliseconds = 100;
        if (syncTimer < 255) syncTimer++;
        clockTenths = (clockTenths < 9) ? (clockTenths + 1) : 0;
    }

    fastFlashState = (clockTenths & 1);
}

static void UpdateUserTimer(void)
{
    // Counts 100ms intervals for userTimer100.
    // Not synchronized to the clock.
    static uint8_t milliseconds = 0;

    if (userTimer100 == 0)
    {
        milliseconds = 0;
    }
    else
    {
        milliseconds++;

        if (milliseconds >= 100)
        {
            milliseconds = 0;
            userTimer100--;
        }
    }
}

static void DebounceButtons(void)
{
    static uint8_t switchTimer1;
    static uint8_t switchTimer2;

#ifdef S3
    static uint8_t switchTimer3;
#endif

    // S1 is non-repeating
    if (S1)
    {
        // button not pressed (active low)
        switchTimer1 = 0;
    }
    else
    {
        if (switchTimer1 < 255) switchTimer1++;
        if (switchTimer1 == SW_PRESS_TIME) pressedS1 = TRUE;
    }

    // S2 is repeating
    if (S2)
    {
        // button not pressed (active low)
        switchTimer2 = 0;
    }
    else
    {
        switchTimer2++;

        if (switchTimer2 == SW_PRESS_TIME)
        {
            pressedS2 = TRUE;
        }
        else if (switchTimer2 >= (SW_PRESS_TIME + SW_REPEAT_TIME))
        {
            switchTimer2 = SW_PRESS_TIME;
            pressedS2 = TRUE;
        }
    }

    flashStateS2 = fastFlashState || (switchTimer2 >= SW_PRESS_TIME);

#ifdef S3
    // S3 is non-repeating
    if (S3)
    {
        // button not pressed (active low)
        switchTimer3 = 0;
    }
    else
    {
        if (switchTimer3 < 255) switchTimer3++;
        if (switchTimer3 == SW_PRESS_TIME) pressedS3 = TRUE;
    }
#endif
}

void ISR_Timer0(void) __interrupt (1)
{
    DisplayUpdateISR();

    static uint8_t timer1ms = TICKS_PER_MS;

    timer1ms--;

    // TICKS_PER_MS is assumed to be larger than the final case value here.
    if (timer1ms == 0)
    {
        timer1ms = TICKS_PER_MS;
        UpdateClockTenths();
    }
    else if (timer1ms == 1)
    {
        DebounceButtons();
    }
    else if (timer1ms == 2)
    {
        UpdateUserTimer();
    }
    else if (timer1ms == 3)
    {
        BuzzerControlISR();
    }
}

__bit CheckResetPressed(void)
{
    userTimer100 = 2;

    while (userTimer100 != 0)
    {
        // wait for keypresses to accumulate
    }

    while (!S1 || !S2)
    {
        // wait for switches released
    }

    __bit result = pressedS1 && pressedS2;
    pressedS1 = FALSE;
    pressedS2 = FALSE;
    return result;
}
