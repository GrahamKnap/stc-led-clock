#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>

// Configuration data, stored in DS1302 battery-backed RAM.
// The maximum size is 31 bytes.
#define CONFIG_MAGIC    0x5A

typedef struct CONFIG_T {
    uint8_t magic;          // [0] CONFIG_MAGIC
    uint8_t flags;          // [1] Use the bit-addressable variables
    uint8_t alarmHour;      // [2] BCD; 0x00..0x23
    uint8_t alarmMinute;    // [3] BCD; 0x00..0x59
    uint8_t chimeStartHour; // [4] BCD; 0x00..0x23
    uint8_t chimeStopHour;  // [5] BCD; 0x00..0x23
    uint8_t brightMaximum;  // [6] BCD; 0x01..0x50
    uint8_t brightMinimum;  // [7] BCD; 0x01..0x50
    int8_t  tempOffset;     // [8] constrain range to +/- 15
    int8_t  driftComp;      // [9] constrain range to +/- 9
} CONFIG;

extern CONFIG config;

// copy of config.flags to allow bit-addressing
extern uint8_t __at 0x2F configFlags;

// bit-addressable configuration flags
extern __bit __at 0x78 alarmOn;     // alarm On/Off status
extern __bit __at 0x79 chimeOn;     // chime On/Off status
extern __bit __at 0x7A rsvdPref2;   // not currently used
extern __bit __at 0x7B rsvdPref3;   // not currently used
extern __bit __at 0x7C autoCycle;   // automatically cycle display
extern __bit __at 0x7D degreesF;    // select degrees F (true) or C (false)
extern __bit __at 0x7E monthFirst;  // month:day (true) or day:month (false) display format
extern __bit __at 0x7F twelveHour;  // = 1 when 12 hr mode

void ResetRtc(void);

#endif // file
