//
// STC15W408AS LED Clock
//

// project definitions
#include "global.h"

// definitions for this file
// (none)

// other local headers
#include "adc.h"
#include "chime.h"
#include "config.h"
#include "display.h"
#include "ds1302.h"
#include "serial.h"
#include "timer.h"
#include "utility.h"

// system headers
#include <stdint.h>
#include "stc15.h"


enum stateTable
{
    stClock,
    msClock, msClockHour, msClockMinute, msClockSeconds, msDriftComp,
    msAlarm, msAlarmToggle, msAlarmHour, msAlarmMinute,
    msChime, msChimeToggle, msChimeStartHour, msChimeStopHour,
    msDate, msYear, msMonth, msDateDay, msDayOfWeek,
    scCfg, msToggle24H, msToggleTempUnits, msToggleDateFormat, msToggleAutoCycle,
    msBrightness, msBrtMax, msBrtMin,
    msTempCal, msSetTemp,
    msSaveConfig, msExit
};

static uint8_t displayState;    // the main control variable
static uint8_t bcdTemp;         // result of ADC and transform
static uint8_t setupTimeout;

static void ClockStateMachine(void)
{
    static __bit driftCompPending = FALSE;
    __bit handleS1;
    __bit handleS2;

    handleS1 = pressedS1;
    pressedS1 = FALSE;
    handleS2 = pressedS2;
    pressedS2 = FALSE;

    switch (displayState)
    {

    case stClock:
        ReadClock();

        if (clock.hour == 0x23 && clock.minute == 0x59)
        {
            if (clock.second == 0)
            {
                driftCompPending = TRUE;
            }
            else if (clock.second == 0x30 && driftCompPending)
            {
                driftCompPending = FALSE;
                clock.second += config.driftComp;
                // BCD subtraction correction
                if (config.driftComp < 0) clock.second -= 6;
                WriteClock();
            }
        }

        if (userTimer100 == 0)
        {
            DisplayHours(clock.hour);
            DisplayHexRight(clock.minute);
            DisplayColon();
            DisplayPM(clock.hour);
        }
        else if (userTimer100 <= 100 || userTimer100 > 175)
        {
            if (userTimer100 > 175) userTimer100 = 250;
            DisplayHexRight(clock.second);
            DisplayColon();
        }
        else if (userTimer100 <= 125)
        {
            DisplayTemperature(bcdTemp);
        }
        else if (userTimer100 <= 150)
        {
            DisplayMonth(clock.month);

            if (monthFirst)
            {
                DisplayHexRight(clock.date);
            }
            else
            {
                DisplayHexLeft(clock.date);
            }
        }
        else    // 151 to 175
        {
            DisplayDayOfWeek();
        }

        if (handleS1)
        {
            userTimer100 = 10;
            setupTimeout = SETUP_TIMEOUT;
            displayState = msClock;
        }
        else if (handleS2)
        {
            if (userTimer100 > 100)
            {
                userTimer100 = 0;
            }
            else
            {
                userTimer100 = (userTimer100 == 0) ? 175 : 250;
            }
        }
        else if (autoCycle && userTimer100 == 0 && clock.second == 0x46)
        {
            userTimer100 = 175;
        }

        break;

    // --- clock settings ---

    case msClock:
        digit[0] = SEVENSEG_C;
        digit[1] = SEVENSEG_L;

        if (handleS1)
        {
            displayState = msAlarm;
        }
        else if (handleS2)
        {
            displayState = msClockHour;
        }

        break;

    case msClockHour:
        if (flashStateS2) DisplayHours(clock.hour);
        DisplayHexRight(clock.minute);
        DisplayColon();
        DisplayPM(clock.hour);

        if (handleS1)
        {
            displayState = msClockMinute;
        }
        else if (handleS2)
        {
            clock.hour = IncrementHours(clock.hour);
        }

        break;

    case msClockMinute:
        DisplayHours(clock.hour);
        if (flashStateS2) DisplayHexRight(clock.minute);
        DisplayColon();
        DisplayPM(clock.hour);

        if (handleS1)
        {
            WriteClock();
            displayState = msClockSeconds;
        }
        else if (handleS2)
        {
            clock.minute = IncrementMinutes(clock.minute);
        }

        break;

    case msClockSeconds:
        ReadClock();
        DisplayHexLeft(clock.minute);
        if (fastFlashState) DisplayHexRight(clock.second);
        DisplayColon();

        if (handleS1)
        {
            displayState = msDriftComp;
        }
        else if (handleS2)
        {
            // snap to nearest minute
            if (clock.second >= 0x30)
            {
                clock.minute = IncrementMinutes(clock.minute);
            }

            clock.second = 0;
            WriteClock();
        }

        break;

    case msDriftComp:
        digit[0] = SEVENSEG_c;
        digit[1] = SEVENSEG_o;

        if (config.driftComp >= 0)
        {
            DisplayHexRight(config.driftComp);
            digit[2] = SEVENSEG_blank;
        }
        else
        {
            DisplayHexRight(-config.driftComp);
            digit[2] = ORIENT_DIGIT2(SEVENSEG_dash);
        }

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            if (config.driftComp >= 9)
            {
                config.driftComp = -9;
            }
            else
            {
                config.driftComp++;
            }
        }

        break;

    // --- alarm settings ---

    case msAlarm:
        digit[0] = SEVENSEG_A;
        digit[1] = SEVENSEG_L;

        if (handleS1)
        {
            displayState = msChime;
        }
        else if (handleS2)
        {
            displayState = msAlarmToggle;
        }

        break;

    case msAlarmToggle:
        digit[0] = SEVENSEG_A;
        digit[1] = SEVENSEG_L;

        if (fastFlashState)
        {
            digit[2] = ORIENT_DIGIT2(SEVENSEG_o);
            digit[3] = (alarmOn) ? SEVENSEG_n : SEVENSEG_F;
        }

        if (handleS1)
        {
            displayState = msAlarmHour;
        }
        else if (handleS2)
        {
            alarmOn = !alarmOn;
        }

        break;

    case msAlarmHour:
        if (flashStateS2) DisplayHours(config.alarmHour);
        DisplayHexRight(config.alarmMinute);
        DisplayColon();
        DisplayPM(config.alarmHour);

        if (handleS1)
        {
            displayState = msAlarmMinute;
        }
        else if (handleS2)
        {
            config.alarmHour = IncrementHours(config.alarmHour);
        }

        break;

    case msAlarmMinute:
        DisplayHours(config.alarmHour);
        if (flashStateS2) DisplayHexRight(config.alarmMinute);
        DisplayColon();
        DisplayPM(config.alarmHour);

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            config.alarmMinute = IncrementMinutes(config.alarmMinute);
        }

        break;

    // --- chime settings ---

    case msChime:
        digit[0] = SEVENSEG_C;
        digit[1] = SEVENSEG_H;

        if (handleS1)
        {
            displayState = msDate;
        }
        else if (handleS2)
        {
            displayState = msChimeToggle;
        }

        break;

    case msChimeToggle:
        digit[0] = SEVENSEG_C;
        digit[1] = SEVENSEG_H;

        if (fastFlashState)
        {
            digit[2] = ORIENT_DIGIT2(SEVENSEG_o);
            digit[3] = (chimeOn) ? SEVENSEG_n : SEVENSEG_F;
        }

        if (handleS1)
        {
            displayState = msChimeStartHour;
        }
        else if (handleS2)
        {
            chimeOn = !chimeOn;
        }

        break;

    case msChimeStartHour:
        if (flashStateS2) DisplayHours(config.chimeStartHour);
        digit[2] = ORIENT_DIGIT2(SEVENSEG_C);
        digit[3] = SEVENSEG_b;
        DisplayPM(config.chimeStartHour);

        if (handleS1)
        {
            displayState = msChimeStopHour;
        }
        else if (handleS2)
        {
            config.chimeStartHour = IncrementHours(config.chimeStartHour);
        }

        break;

    case msChimeStopHour:
        if (flashStateS2) DisplayHours(config.chimeStopHour);
        digit[2] = ORIENT_DIGIT2(SEVENSEG_C);
        digit[3] = SEVENSEG_E;
        DisplayPM(config.chimeStopHour);

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            config.chimeStopHour = IncrementHours(config.chimeStopHour);
        }

        break;

    // --- date settings ---

    case msDate:
        digit[0] = SEVENSEG_d;
        digit[1] = SEVENSEG_A;
        digit[2] = SEVENSEG_t;
        digit[3] = SEVENSEG_E;

        if (handleS1)
        {
            displayState = scCfg;
        }
        else if (handleS2)
        {
            displayState = msYear;
        }

        break;

    case msYear:
        digit[0] = SEVENSEG_y;
        digit[1] = SEVENSEG_r;
        if (fastFlashState) DisplayHexRight(clock.year);

        if (handleS1)
        {
            displayState = msMonth;
        }
        else if (handleS2)
        { 
            clock.year = IncrementBcd(clock.year);
        }

        break;

    case msMonth:
        if (flashStateS2) DisplayMonth(clock.month);

        if (monthFirst)
        {
            DisplayHexRight(clock.date);
        }
        else
        {
            DisplayHexLeft(clock.date);
        }

        if (handleS1)
        {
            uint8_t maxDate = DaysInMonth(clock.month);
            if (clock.date > maxDate) clock.date = maxDate;
            displayState = msDateDay;
        }
        else if (handleS2)
        {
            uint8_t month = IncrementBcd(clock.month);
            clock.month = (month <= 0x12) ? month : 1;
        }

        break;

    case msDateDay:
        DisplayMonth(clock.month);

        if (flashStateS2)
        {
            if (monthFirst)
            {
                DisplayHexRight(clock.date);
            }
            else
            {
                DisplayHexLeft(clock.date);
            }
        }

        if (handleS1)
        {
            displayState = msDayOfWeek;
        }
        else if (handleS2)
        {
            uint8_t maxDate = DaysInMonth(clock.month);
            clock.date = (clock.date < maxDate) ? IncrementBcd(clock.date) : 0x01;
        }

        break;

    case msDayOfWeek:
        if (flashStateS2)
        {
#ifdef OPT_DAY_ALPHA
            DisplayDayOfWeek();
#else
            DisplayRightHex(clock.weekday);
            digit[0] = SEVENSEG_d;
            digit[1] = SEVENSEG_A;
            digit[2] = ORIENT_DIGIT2(SEVENSEG_y);
#endif
        }

        if (handleS1)
        {
            WriteDate();
            displayState = msExit;
        }
        else if (handleS2)
        {
            clock.weekday = (clock.weekday < 7) ? (clock.weekday + 1) : 1;
        }

        break;

    // --- configuration settings ---

    case scCfg:
        digit[0] = SEVENSEG_C;
        digit[1] = SEVENSEG_F;
        digit[2] = ORIENT_DIGIT2(SEVENSEG_g);
        digit[3] = 0;

        if (handleS1)
        {
            displayState = msBrightness;
        }
        else if (handleS2)
        {
            displayState = msToggle24H;
        }

        break;

    case msToggle24H:
        if (fastFlashState)
        {
            DisplayHexLeft((twelveHour) ? 0x12 : 0x24);
            digit[2] = ORIENT_DIGIT2(SEVENSEG_H);
        }

        if (handleS1)
        {
            displayState = msToggleTempUnits;
        }
        else if (handleS2)
        {
            twelveHour = !twelveHour;
        }

        break;

    case msToggleTempUnits:
        if (fastFlashState)
        {
            digit[0] = SEVENSEG_d;
            digit[1] = SEVENSEG_E;
            digit[2] = ORIENT_DIGIT2(SEVENSEG_g);
            digit[3] = (degreesF) ? SEVENSEG_F : SEVENSEG_C;
        }

        if (handleS1)
        {
            displayState = msToggleDateFormat;
        }
        else if (handleS2)
        {
            degreesF = !degreesF;
        }

        break;

    case msToggleDateFormat:
        if (fastFlashState)
        {
            DisplayMonth(0x12);

            if (monthFirst)
            {
                DisplayHexRight(0x31);
            }
            else
            {
                DisplayHexLeft(0x31);
            }
        }

        if (handleS1)
        {
            displayState = msToggleAutoCycle;
        }
        else if (handleS2)
        {
            monthFirst = !monthFirst;
        }

        break;

    case msToggleAutoCycle:
        digit[0] = SEVENSEG_A;
        digit[1] = SEVENSEG_C;

        if (fastFlashState)
        {
            digit[2] = ORIENT_DIGIT2(SEVENSEG_o);
            digit[3] = (autoCycle) ? SEVENSEG_n : SEVENSEG_F;
        }

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            autoCycle = !autoCycle;
        }

        break;

    // --- brightness settings ---

    case msBrightness:
        digit[0] = SEVENSEG_b;
        digit[1] = SEVENSEG_r;
        digit[2] = ORIENT_DIGIT2(SEVENSEG_t);
        digit[3] = 0;

        if (handleS1)
        {
            displayState = msTempCal;
        }
        else if (handleS2)
        {
            displayState = msBrtMax;
        }

        break;

    case msBrtMax:
        digit[0] = SEVENSEG_b;
        digit[1] = SEVENSEG_H;
        if (flashStateS2) DisplayHexRight(config.brightMaximum);

        if (handleS1)
        {
            displayState = msBrtMin;
        }
        else if (handleS2)
        {
            config.brightMaximum = IncrementBcd(config.brightMaximum);

            if (config.brightMaximum >= DecToBcd(DISPLAY_TICKS))
            {
                config.brightMaximum = 0x01;
                config.brightMinimum = 0x01;
            }

#ifndef HAS_LDR
            brightLevel = BcdToDec(clock.brightMaximum);
#endif
        }

        break;

    case msBrtMin:
        digit[0] = SEVENSEG_b;
        digit[1] = SEVENSEG_L;
        if (flashStateS2) DisplayHexRight(config.brightMinimum);

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            config.brightMinimum = IncrementBcd(config.brightMinimum);

            if (config.brightMinimum > config.brightMaximum)
            {
                config.brightMinimum = 0x01;
            }
        }

        break;

    // --- temperature settings ---

    case msTempCal:
        digit[0] = SEVENSEG_C;
        digit[1] = SEVENSEG_A;
        digit[2] = ORIENT_DIGIT2(SEVENSEG_L);
        digit[3] = 0;

        if (handleS1)
        {
            displayState = msExit;
        }
        else if (handleS2)
        {
            displayState = msSetTemp;
        }

        break;

    case msSetTemp:
        if (flashStateS2) DisplayTemperature(bcdTemp);

        if (handleS1)
        {
            displayState = msSaveConfig;
        }
        else if (handleS2)
        {
            if (config.tempOffset == 15)
            {
                config.tempOffset = -15;
            }
            else
            {
                config.tempOffset += 1;
            }
        }

        break;

    // --- save/exit configuration ---

    case msSaveConfig:
        config.flags = configFlags;
        WriteConfig();
        // no "break", fall through to default

    case msExit:
    default:
        userTimer100 = 0;
        displayState = stClock;
        break;
    }

    // With no code below the 'switch' block, each 'break' statement compiles
    // to a 1-byte 'ret' instruction. Otherwise, it's a 3-byte 'ljmp'.
}

static void ReadSensors(void)
{
    static uint8_t roundRobin = 0;

    if (adcBusy)
    {
        return;
    }

    if (roundRobin == 0)
    {
        bcdTemp = DecToBcd(DecodeTemperature(adcResult) + config.tempOffset);
#ifdef HAS_LDR
        roundRobin = 1;
        StartADC(ADC_LDR);
    }
    else if (roundRobin == 1)
    {
        brightLevel = (brightLevel + MapBrightness(adcResult) + 1) >> 1;
        roundRobin = 0;
#endif
        StartADC(ADC_TEMP);
    }
}

int main()
{
#ifdef DEBUG
    InitSerial();
    PrintString("Start\r\n");
#endif

    InitDisplay();

#ifdef HAS_LDR
    InitADC(ADC_LDR);
#endif

    InitADC(ADC_TEMP);
    InitRtc();
    ReadConfig();
    configFlags = config.flags;
    InitTimer0();

#ifdef TEST_DEFAULTS
    ResetRtc();
#else
    if (config.magic != CONFIG_MAGIC || CheckResetPressed())
    {
        ResetRtc();
    }
#endif

#ifdef HAS_NY3P_SPEECH
    InitSpeech();
#endif

    while (TRUE)
    {
        digit[0] = 0;
        digit[1] = 0;
        digit[2] = 0;
        digit[3] = 0;

        UpdateBuzzer(displayState == stClock);
        ReadSensors();

        if (displayState != stClock)
        {
            if (pressedS1 || pressedS2)
            {
                setupTimeout = SETUP_TIMEOUT;
                userTimer100 = 10;
            }
            else if (userTimer100 == 0)
            {
                if (setupTimeout != 0)
                {
                    setupTimeout--;
                    userTimer100 = 10;
                }

                // setup mode ignored too long, abort setup
                if (setupTimeout == 0)
                {
                    displayState = stClock;
                    userTimer100 = 0;
                }
            }
        }

#ifdef HAS_NY3P_SPEECH
        if (displayState == stClock && pressedS3)
        {
            speakTime();
            pressedS3 = FALSE;
        }
#endif

        ClockStateMachine();
        CommitDisplay();
    }
}
