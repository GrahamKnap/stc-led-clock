// project definitions
#include "global.h"

// definitions for this file
#include "sound.h"

// other local headers
#include "display.h"
#include "utility.h"
#include "ds1302.h"

// system headers
#include "stc15.h"

#if HAS_NY3P_SPEECH

void speakTime()
{
    uint8_t h,m;

    if (Select_12){
        h = bcdToDec(clock.hour & 0x1F);
        m = bcdToDec(clock.minute);
        if ( m == 0 ){
        // just speak hour and AM/PM
            speakItem(h);
            speakAM_PM();
        }
        else if ( m >= 1 & m <= 9){
        // hour + "oh" + "nine" + am/pm
            speakItem(h);
            speakItem(sndOh);
            speakItem(m);
            speakAM_PM();
        }
        else if ( m >= 10 & m <= 20){
        // hour + minute + am/pm
            speakItem(h);
            speakItem(m);
            speakAM_PM();
        }
        else {//if ( m >= 21 & m <= 59)
        // hour + minuteTems + minute + am/pm
            speakItem(h);
            speakItem(m/10+sndTwenty-2);
            speakItem(m % 10);
            speakAM_PM();
        }
    }

    if (!Select_12){ // 24 hour time here
        h = bcdToDec(clock.hour);
        m = bcdToDec(clock.minute);
        if ( m == 0 ){
        // just speak hour and ohClock
            speakItem(h);
            speakItem(sndOhClock);
        }
        else if ( m >= 1 & m <= 9){
        // hour + "oh" + minute
            speakItem(h);
            speakItem(sndOh);
            speakItem(m);
        }
        else if ( m >= 10 & m <= 20){
        // hour + minute
            speakItem(h);
            speakItem(m);
        }
        else {//if ( m >= 21 & m <= 59)
        // hour + minuteTens + minute
            speakItem(h);
            speakItem(m/10+sndTwenty-2);
            speakItem(m % 10);
        }
    }
}

void speakAM_PM()
{
    if (AM_PM)
        speakItem(sndPM);
    else
        speakItem(sndAM);
}

void speakItem(uint8_t bitCount)
{
    uint8_t i;

    resetSound();
    for( i = 0; i < bitCount; i++ )
        sendOneBit();
    while (NY3P_BZY)
        updateClock();
    resetSound();
}

void sendOneBit()
{
    NY3P_DAT = 1;
    waitS1Clk();
    NY3P_DAT = 0;
    waitS1Clk();
}

void waitS1Clk()
{
    soundTimer = 5;             // 250us pulses
    while( soundTimer ) ;
}

void resetSound()
{
    NY3P_RST = 0;               // should be at zero but force anyway
    waitS1Clk();
    NY3P_RST = 1;               // =1 is the actual reset
    waitS1Clk();
    NY3P_RST = 0;               // and exit with =0
    waitS1Clk();
}

#endif // HAS_NY3P_SPEECH

