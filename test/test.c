// project definitions
// (none)

// definitions for this file
// (none)

// local headers
#include "../src/config.h"
#include "../src/display.h"
#include "../src/ds1302.h"
#include "../src/utility.h"

// system headers
#include <stdio.h>
#include <stdint.h>

#define COUNTOF(array) (sizeof(array) / sizeof((array)[0]))

// http://mazsola.iit.uni-miskolc.hu/~drdani/embedded/ucsim/simif.html
static __xdata char * simif = (__xdata char *)0xffff;

int putchar(int c)
{
    *simif = 'p';
    *simif = c;
    return c;
}

static void TestBcdToDec(void)
{
    static const uint8_t bcd[] = { 0x00, 0x09, 0x42, 0x99 };
    static const uint8_t dec[] = { 0, 9, 42, 99 };
    uint8_t i, r;

    for (i = 0; i < COUNTOF(bcd); i++)
    {
        r = BcdToDec(bcd[i]);

        if (r != dec[i])
        {
            printf_tiny("BcdToDec(0x%x): want %u, got %u\n",
                bcd[i], dec[i], r);
        }

        r = DecToBcd(dec[i]);

        if (r != bcd[i])
        {
            printf_tiny("DecToBcd(%u): want 0x%x, got 0x%x\n",
                dec[i], bcd[i], r);
        }
    }
}

static void TestDecodeTemperature(void)
{
    static const uint8_t adc[] = { 255, 189, 128, 70, 0 };
    static const uint8_t degC[] = { 0, 0, 26, 51, 81 };
    static const uint8_t degF[] = { 0, 32, 79, 124, 177 };
    uint8_t i, r;

    for (i = 0; i < COUNTOF(adc); i++)
    {
        degreesF = 1;
        r = DecodeTemperature(adc[i]);

        if (r != degF[i])
        {
            printf_tiny("DecodeTemperature(%u): want %u F, got %u\n",
                adc[i], degF[i], r);
        }

        degreesF = 0;
        r = DecodeTemperature(adc[i]);

        if (r != degC[i])
        {
            printf_tiny("DecodeTemperature(%u): want %u C, got %u\n",
                adc[i], degC[i], r);
        }
    }
}

static void TestIncrementBcd(void)
{
    static const uint8_t x[] = { 0x00, 0x59, 0x98, 0x99 };
    static const uint8_t y[] = { 0x01, 0x60, 0x99, 0x00 };
    uint8_t i, r;

    for (i = 0; i < COUNTOF(x); i++)
    {
        r = IncrementBcd(x[i]);

        if (r != y[i])
        {
            printf_tiny("IncrementBcd(0x%x): want 0x%x, got 0x%x\n",
                x[i], y[i], r);
        }
    }
}

static void TestMapBrightness(void)
{
    static const uint8_t adc[] = { 0, 77, 123, 222, 255 };
    static const uint8_t brt[] = { 70, 55, 46, 26, 20 };
    uint8_t i, r;

    config.brightMinimum = 0x20;
    config.brightMaximum = 0x70;

    for (i = 0; i < COUNTOF(adc); i++)
    {
        r = MapBrightness(adc[i]);

        if (r != brt[i])
        {
            printf_tiny("MapBrightness(%u): want %u, got %u\n",
                adc[i], brt[i], r);
        }
    }
}

/*
static void TestRotateDigit(void)
{
    static const uint8_t d[] = { 0x79, 0x87 };
    static const uint8_t f[] = { 0x4f, 0xb8 };
    uint8_t i, r;

    for (i = 0; i < COUNTOF(d); i++)
    {
        r = RotateDigit(d[i]);

        if (r != f[i])
        {
            printf_tiny("RotateDigit(0x%x): want 0x%x, got 0x%x\n",
                d[i], f[i], r);
        }
    }
}
*/

void main(void)
{
    printf_tiny("Test program started\n");

    TestBcdToDec();
    TestDecodeTemperature();
    TestIncrementBcd();
    TestMapBrightness();
    //TestRotateDigit();

    printf_tiny("Test program complete\n");
    *simif = 's';

    while (1)
    {
        // halt
    }
}
