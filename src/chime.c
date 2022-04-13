// project definitions
#include "global.h"

// definitions for this file
#include "chime.h"

// other local headers
#include "config.h"
#include "ds1302.h"
#include "timer.h"

// system headers
#include <stdint.h>
#include "stc15.h"


static uint8_t buzzCount;
static __bit buzzTriggered;

static __bit IsHourInChimeRange(void)
{
    if (config.chimeStartHour <= config.chimeStopHour)
    {
        // day chime
        return (config.chimeStartHour <= clock.hour
            && clock.hour <= config.chimeStopHour);
    }
    else
    {
        // night chime
        return (config.chimeStartHour <= clock.hour
            || clock.hour <= config.chimeStopHour);
    }
}

void UpdateBuzzer(__bit enable)
{
    static uint8_t prevTenths;
    static uint8_t buzzTimer;

    if (clock.second >= 3)
    {
        buzzTriggered = FALSE;
    }
    else if (enable && !buzzTriggered)
    {
        if (alarmOn
            && clock.hour == config.alarmHour
            && clock.minute == config.alarmMinute)
        {
            buzzCount = 240;
            buzzTriggered = TRUE;
        }
        else if (chimeOn
            && IsHourInChimeRange()
            && clock.minute == 0)
        {
            buzzCount = 1;
            buzzTriggered = TRUE;
        }
    }

    if (buzzCount != 0 && (pressedS1 || pressedS2))
    {
        buzzCount = 0;
        pressedS1 = FALSE;
        pressedS2 = FALSE;
    }
}

// This is called by the timer ISR once per millisecond.
void BuzzerControlISR(void)
{
    static uint8_t milliseconds;
    static uint8_t buzzTimer;

    if (buzzCount == 0)
    {
        buzzTimer = 0;
        milliseconds = 0;
        BUZZ_SBIT = !BUZZ_ON;
    }
    else
    {
        if (buzzTimer == 0 || buzzTimer == 3)
        {
            BUZZ_SBIT = BUZZ_ON;
        }
        else
        {
            BUZZ_SBIT = !BUZZ_ON;
        }

        milliseconds++;

        if (milliseconds >= 100)
        {
            milliseconds = 0;
            buzzTimer++;

            if (buzzTimer >= 10)
            {
                buzzTimer = 0;
                buzzCount--;
            }
        }
    }
}
