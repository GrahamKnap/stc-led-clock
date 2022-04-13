// project definitions
#include "global.h"

// definitions for this file
#include "utility.h"

// other local headers
#include "config.h"
#include "display.h"
#include "ds1302.h"

// system headers
// (none)


uint8_t MapBrightness(uint8_t adcValue)
{
    uint8_t min, range;

    adcValue = 255 - adcValue;
    min = BcdToDec(config.brightMinimum);
    range = BcdToDec(config.brightMaximum) - min;
    return min + (((range * adcValue) + (1 << 7)) >> 8);
}

// Thermistors are normally nonlinear.
// This is only intended to produce good results near 20C.

uint8_t DecodeTemperature(uint8_t adcValue)
{
    uint8_t b, m, temp;

    if (degreesF)
    {
        // approx 0.77 F per count, represented as 197/256
        b = (uint8_t)177;
        m = (uint8_t)197;
    }
    else
    {
        // approx 0.43 C per count, represented as 110/256
        b = (uint8_t)81;
        m = (uint8_t)110;
    }

    temp = (m * adcValue) >> 8;
    return (b > temp) ? (b - temp) : (uint8_t)0;
}

// Returns the decoded decimal value from a binary-coded decimal (BCD) byte.

uint8_t BcdToDec(uint8_t bcd)
{
    return ((uint8_t)10 * (bcd >> 4)) + (bcd & 0x0f);
}

// Encodes a number (maximum 99) as binary-coded decimal.

uint8_t DecToBcd(uint8_t dec)
{
    uint8_t tens = dec / (uint8_t)10;
    uint8_t ones = dec - ((uint8_t)10 * tens);
    return (tens << 4) | ones;
}

uint8_t IncrementBcd(uint8_t value)
{
    value++;

    if (value > 0x99)
    {
        value = 0;
    }
    else if ((value & 0x0f) > 9)
    {
        value = (value & 0xf0) + 0x10;
    }

    return value;
}

uint8_t IncrementHours(uint8_t h)
{
    return (h < 0x23) ? IncrementBcd(h) : 0;
}

uint8_t IncrementMinutes(uint8_t m)
{
    return (m < 0x59) ? IncrementBcd(m) : 0;
}

// The argument must be a BCD value from 0x00 to 0x23.
uint8_t ConvertHours(uint8_t value)
{
    if (twelveHour)
    {
        if (value == 0)
        {
            value = 0x12;
        }
        else if (value == 0x20 || value == 0x21)
        {
            // 0x20 becomes 0x08
            // 0x21 becomes 0x09
            value -= 0x18;
        }
        else if (value >= 0x13)
        {
            // 0x13 becomes 0x01
            // 0x19 becomes 0x07
            // 0x22 becomes 0x10
            // 0x23 becomes 0x11
            value -= 0x12;
        }
    }

    return value;
}

// The argument must be a BCD value from 0x01 to 0x12.
// The return value is in BCD format.
uint8_t DaysInMonth(uint8_t m)
{
    if (m == 0x02)
    {
        // The clock stores only a 2-digit year,
        // so this ignores the 100-year and 400-year rules,
        // and will produce an incorrect result in year 2100.
        return (BcdToDec(clock.year) & 3) == 0 ? 0x29 : 0x28;
    }
    else if (m == 0x04 || m == 0x06 || m == 0x09 || m == 0x11)
    {
        return 0x30;
    }
    else
    {
        return 0x31;
    }
}
