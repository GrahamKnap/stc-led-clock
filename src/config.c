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

void ResetRtc(void)
{
    // You can adjust these defaults to your liking.
    // Declaring invalid values will usually result in just strange
    // displays, but may cause crashes since the data is not validated.

    clock.second = 0x55;    // [0] BCD; 0x00..0x59
    clock.minute = 0x59;    // [1] BCD; 0x00..0x59
    clock.hour = 0x19;      // [2] BCD; 0x00..0x23
    clock.date = 0x01;      // [3] day of month; BCD; 0x01..0x31
    clock.month = 0x01;     // [4] BCD; 0x01..0x12
    clock.weekday = 1;      // [5] 1..7
    clock.year = 23;        // [6] BCD; 0x00..0x99
    WriteClock();

    config.magic = CONFIG_MAGIC;    // [0]
    config.flags = TWELVE_HOUR | MONTH_FIRST;
    config.alarmHour = 0x07;        // [2] BCD; 0x00..0x23
    config.alarmMinute = 0x00;      // [3] BCD; 0x00..0x59
    config.chimeStartHour = 0x08;   // [4] BCD; 0x00..0x23
    config.chimeStopHour = 0x21;    // [5] BCD; 0x00..0x23
    config.brightMaximum = 0x30;    // [6] BCD; 0x01..0x50
    config.brightMinimum = 0x01;    // [7] BCD; 0x01..0x50
    config.tempOffset = 0;          // [8] -15 to +15
    config.driftComp = 0;           // [9] -9 to +9
    configFlags = config.flags;
    WriteConfig();
}
