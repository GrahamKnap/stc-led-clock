// project definitions
#include "global.h"

// definitions for this file
#include "config.h"

// other local headers
#include "ds1302.h"


CONFIG config;

// Declare this byte absolute 0x2F which memory maps it in the 8051's bit space
// begining at 0x78. Then declare the bits used in that byte so they can be accessed
// without having to mask and shift...

uint8_t __at 0x2F configFlags;
__bit __at 0x78 alarmOn;    // alarm On/Off status
__bit __at 0x79 chimeOn;    // chime On/Off status
__bit __at 0x7A rsvdPref2;  // currently not used
__bit __at 0x7B rsvdPref3;  // currently not used
__bit __at 0x7C autoCycle;  // automatically cycle the display
__bit __at 0x7D degreesF;   // select degrees F or C
__bit __at 0x7E monthFirst; // select month:day display format MM:DD or DD:MM
__bit __at 0x7F twelveHour; // = 1 when 12 hr mode

// bit definitions used in configuration bits
#define ALARM_ON     0x01   // alarm On/Off status
#define CHIME_ON     0x02   // chime On/Off status
#define RSVD_PREF_2  0x04   // currently not used
#define RSVD_PREF_3  0x08   // currently not used
#define AUTO_CYCLE   0x10   // automatically cycle the display
#define DEGREES_F    0x20   // select degrees F or C
#define MONTH_FIRST  0x40   // select month:day display format MM:DD or DD:MM
#define TWELVE_HOUR  0x80   // = 1 when 12 hr mode

// The coldstart initialization table.
// Here you can set the reset at power up defaults to your liking.
// Note that most of the values are in BCD.
// Declaring invalid values will usually result in just strange
// displays - but may cause crashes since the data is not validated.

static const CLOCK defaultClock = {
    0x55, 0x59, 0x19,                   // second, minute, hour
    0x01, 0x01,                         // date, month
    0x07,                               // day of week (1-7)
    0x22                                // 2-digit year
};

static const CONFIG defaultConfig = {
    CONFIG_MAGIC,                       // check byte
    TWELVE_HOUR | MONTH_FIRST,          // mode bits
    0x07, 0x00,                         // alarm hour, minute
    0x08,                               // chime start hour
    0x21,                               // chime stop hour
    0x31, 0x01,                         // brightness max/min
    0x00                                // temp offset
};

void ResetRtc(void)
{
    uint8_t i;

    // we could use memcpy() here, but it's much larger
    for (i = 0; i < sizeof(CLOCK); i++)
    {
        ((uint8_t *)&clock)[i] = ((uint8_t *)&defaultClock)[i];
    }

    for (i = 0; i < sizeof(CONFIG); i++)
    {
        ((uint8_t *)&config)[i] = ((uint8_t *)&defaultConfig)[i];
    }

    configFlags = config.flags;
    WriteClock();
    WriteConfig();
}
