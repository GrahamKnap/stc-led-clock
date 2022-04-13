
#ifndef _SOUND_H
#define _SOUND_H

#if HAS_NY3P_SPEECH

#include <stdint.h>

enum soundsIndex {
    sndOne = 1,
    sndTwo,
    sndThree,
    sndFour,
    sndFive,
    sndSix,
    sndSeven,
    sndEight,
    sndNine,
    sndTen,
    sndEleven,
    sndTwelve,
    sndThirteen,
    sndFourteen,
    sndFifteen,
    sndSixteen,
    sndSeventeen,
    sndEighteen,
    sndNineteen,
    sndTwenty,
    sndThirty,
    sndFourty,
    sndFifty,
    sndOhClock,
    sndAh,
    sndOh,
    sndAM,
    sndPM,
    sndChime,
    sndJingle,
    sndRing
};

void speakTime();
void speakItem(uint8_t bitCount);
void speakAM_PM();
void sendOneBit();
void resetSound();
void waitS1Clk();

#endif
#endif
