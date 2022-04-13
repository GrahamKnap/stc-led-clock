// project definitions
#include "global.h"

#ifdef DEBUG

// definitions for this file
#include "serial.h"

// other local headers
// (none)

// system headers
#include "stc15.h"
#include <stdio.h>

// AUXR bits
#define T0DIV1  0x80  // T0x12
#define T1DIV1  0x40  // T1x12
#define S1DIV2  0x20  // UART_M0x6
#define T2RUN   0x10  // T2R
#define T2COUNT 0x08  // T2_C/nT
#define T2DIV1  0x04  // T2x12
#define NAUXRAM 0x02  // EXTRAM
#define S1ST2   0x01

// AUXR1/P_SW1 bits
#define S1_S1   0x80
#define S1_S0   0x40

// At 115200 bps, each bit takes ~8.68 us.
// Each byte (start, 8 data bits, stop) takes almost 87 us.

void InitSerial(void)
{
    // SCON register
    // 8-bit UART, variable baud rate
    SM0 = 0;
    SM1 = 1;
    // REN (receive enable) is left at its default of 0.

    // AUXR1/P_SW1 register
    // S1_S1 = 0, S1_S0 = 0: UART1 on P3.0/P3.1
    // S1_S1 = 0, S1_S0 = 1: UART1 on P3.6/P3.7
    // S1_S1 = 1, S1_S0 = 0: UART1 on P1.6/P1.7
    P_SW1 = (P_SW1 & ~(S1_S1)) | (S1_S0);

    // When UART1 uses Timer 2 in 1T mode:
    // baud = SYSclk / (65536 - reloadValue) / 4
    #define BAUD 115200
    #define T2RELOAD (65536 - (FOSC / (BAUD * 4)))
    T2H = T2RELOAD >> 8;
    T2L = T2RELOAD & 0xff;

    // AUXR register
    // * select Timer 2 as the baud-rate generator of UART1
    // * clock source of Timer 2 is SYSclk / 1
    // * run Timer 2
    AUXR |= (S1ST2 | T2DIV1 | T2RUN);

    // IE (interrupt enable) register
    // PS is left at its default of 0 (low UART1 interrupt priority).
    ES = 1;         // enable serial port 1 interrupt
    EA = 1;         // enable interrupts
}

static char txQueue[8];
static uint8_t txStart;
static uint8_t txCount;
static __bit txBusy;

void ISR_UART1(void) __interrupt 4 __using 1
{
    if (RI)
    {
        RI = 0;
    }

    if (TI)
    {
        TI = 0;

        if (txCount == 0)
        {
            txBusy = 0;
        }
        else
        {
            SBUF = txQueue[txStart];
            txStart = (txStart + 1) & 7;
            txCount--;
        }
    }
}

void PrintChar(uint8_t c)
{
    while (txCount == 8)
    {
        // wait until queue is not full
    }

    ES = 0;

    if (txBusy)
    {
        txQueue[(txStart + txCount) & 7] = c;
        txCount++;
    }
    else
    {
        txBusy = 1;
        SBUF = c;
    }

    ES = 1;
}

static void PrintHexDigit(uint8_t n)
{
    n += (n > 9) ? ('A' - 10) : '0';
    PrintChar(n);
}

void PrintHex(uint8_t value)
{
    PrintHexDigit(value >> 4);
    PrintHexDigit(value & 0x0f);
}

void PrintString(const char * s)
{
    while (*s != '\0')
    {
        PrintChar(*s);
        s++;
    }
}

// See function prototype in <stdio.h>
int putchar(int c)
{
    PrintChar(c);
    return c;
}

#else  // not DEBUG

// This function compiles to a single 1-byte 'ret' instruction.
// Its presence avoids an "empty source file" warning.
void InitSerial(void)
{
    // nothing
}

#endif // DEBUG (entire file)
