#ifndef _DS1302_H
#define _DS1302_H

#include <stdint.h>

// Time structure as used by DS1302
// We always store the time in 24-hour format.
// It can be displayed in 12-hour format when needed.

typedef struct CLOCK_T {
    uint8_t second;         // [0] BCD; 0x00..0x59
    uint8_t minute;         // [1] BCD; 0x00..0x59
    uint8_t hour;           // [2] BCD; 0x00..0x23
    uint8_t date;           // [3] day of month; BCD; 0x01..0x31
    uint8_t month;          // [4] BCD; 0x01..0x12
    uint8_t weekday;        // [5] 1..7
    uint8_t year;           // [6] BCD; 0x00..0x99
    uint8_t wp;             // [7] write-protect register
} CLOCK;

extern CLOCK clock;

void ReadClock(void);
void WriteClock(void);
void WriteDate(void);
void InitRtc(void);
void ReadConfig(void);
void WriteConfig(void);

#endif
