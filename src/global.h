
#ifndef _GLOBAL_H
#define _GLOBAL_H

#define TRUE    1                   // for general use, bit setters too
#define FALSE   0

//---------------------------------------------------------------------------
// Begin Software Option configuration
//---------------------------------------------------------------------------

#define OPT_DAY_ALPHA   TRUE    // display days as characters
#define OPT_MONTH_ALPHA TRUE
#define OPT_BLANK_ZERO  TRUE    // T = 1:00 F = 01:00
#define SETUP_TIMEOUT   60      // seconds
//#define DEBUG                 // serial debugging
//#define TEST_DEFAULTS         // load defaults at every power-up

//---------------------------------------------------------------------------
// End Software Option configuration
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Begin Hardware Option configuration
//---------------------------------------------------------------------------

#define FOSC    22118400L           // clock speed in Hz

#define BOARD_TALKING       FALSE
#define BOARD_BLUE_6        FALSE
#define BOARD_BLUE_5_RELAY  FALSE
#define BOARD_YELLOW_5      FALSE
#define BOARD_YELLOW_SMALL  FALSE
#define BOARD_WHITE_SMALL   TRUE
#define BOARD_GREEN_SMALL   FALSE

#define PROC_IS_15W404AS    TRUE    // or 15W408AS
#define PROC_IS_15F204EA    FALSE

#define HAS_LDR             TRUE    // light-dependent resistor

#define COMMON_ANODE        TRUE    // else common cathode
#define DIGIT_2_FLIP        FALSE

//---------------------------------------------------------------------------
// End Hardware Option configuration
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Begin board hardware defines
//---------------------------------------------------------------------------

#if BOARD_TALKING

#define HAS_NY3P_SPEECH TRUE
#define NY3P_RST    P3_6
#define NY3P_DAT    P3_7
#define NY3P_BZY    P1_3

// Pushbutton port pins
#define S1 P3_1
#define S2 P3_0
#define S3 P1_4

// DS1302 pin to port mapping
#define DS1302_CE_SBIT      P1_0
#define DS1302_IO_SBIT      P1_1
#define DS1302_SCK_SBIT     P1_2

// adc channels for sensors
#define ADC_LDR   6
#define ADC_TEMP  7

// buzzer port pins and active state set
// This is an LED on the talking model
#define BUZZ_SBIT   P1_5
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b00111100
#define LED_COMMON_BIT(pos) (2 + (pos))
#define LED_SEGMENT_PORT    P2

#endif  // board_talking

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_BLUE_6

// Pushbutton port pins
#define S2 P3_0
#define S1 P3_1

// DS1302 pin to port mapping
#define DS1302_CE_SBIT      P1_0
#define DS1302_IO_SBIT      P1_1
#define DS1302_SCK_SBIT     P1_2

// adc channels for sensors
#define ADC_LDR   6
#define ADC_TEMP  7

// buzzer port pins and active state set
#define BUZZ_SBIT   P1_5
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b00111100
#define LED_COMMON_BIT(pos) (2 + (pos)
#define LED_SEGMENT_PORT    P2

#endif  // board_blue_6

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_BLUE_5_RELAY

// Pushbutton port pins
#define S1 P3_0
#define S2 P3_1

// DS1302 pin to port mapping
#if PROC_IS_15W404AS
    #define DS1302_CE_SBIT      P5_4
    #define DS1302_IO_SBIT      P5_5
    #define DS1302_SCK_SBIT     P3_2
#elif PROC_IS_15F204EA
    #define DS1302_CE_SBIT      P0_0
    #define DS1302_IO_SBIT      P0_1
    #define DS1302_SCK_SBIT     P3_2
#endif

// adc channels for sensors
#define ADC_LDR   6
#define ADC_TEMP  3

// buzzer port pins and active state set
#define BUZZ_SBIT   P3_3
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b11110000
#define LED_COMMON_BIT(pos) (4 + (pos))
#define LED_SEGMENT_PORT    P2

#endif  // board_blue_5_relay

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_YELLOW_5

// Pushbutton port pins
#define S2 P3_0
#define S1 P3_1

// DS1302 pin to port mapping
#if PROC_IS_15W404AS
    #define DS1302_CE_SBIT      P5_4
    #define DS1302_IO_SBIT      P5_5
    #define DS1302_SCK_SBIT     P3_2
#elif PROC_IS_15F204EA
    // not setup yet
#endif

// adc channels for sensors
#define ADC_LDR   7
#define ADC_TEMP  6

// buzzer port pins and active state set
#define BUZZ_SBIT   P3_3
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b11110000
#define LED_COMMON_BIT(pos) (4 + (pos))
#define LED_SEGMENT_PORT    P2

#endif  // board_yellow_5

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_WHITE_SMALL

#define DP1_IS_COLON TRUE

// Pushbutton port pins
#define S2 P3_0
#define S1 P3_1

// DS1302 pin to port mapping
#if 1
#define DS1302_CE_SBIT      P1_0
#define DS1302_IO_SBIT      P1_1
#define DS1302_SCK_SBIT     P1_2
#else
// for testing
#define DS1302_CE_SFR       P1
#define DS1302_CE_BIT       0
#define DS1302_IO_SFR       P1
#define DS1302_IO_BIT       1
#define DS1302_SCK_SFR      P1
#define DS1302_SCK_BIT      2
#endif

// adc channels for sensors
#define ADC_LDR   6
#define ADC_TEMP  7

// buzzer port pins and active state set
#define BUZZ_SBIT   P1_5
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b00111100
#define LED_COMMON_BIT(pos) (2 + (pos))
#define LED_SEGMENT_PORT    P2

#endif  // board_white_small

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_YELLOW_SMALL

#define DP1_IS_COLON TRUE

// Pushbutton port pins
#define S2 P2_6
#define S1 P2_7

// DS1302 pin to port mapping
#define DS1302_SCK_SBIT     P1_0
#define DS1302_CE_SBIT      P1_1
#define DS1302_IO_SBIT      P1_2

// adc channels for sensors
#define ADC_LDR   6
#define ADC_TEMP  7

// buzzer port pins and active state set
#define BUZZ_SBIT   P5_5
#define BUZZ_ON     0

#define LED_COMMON_PORT     P2
#define LED_COMMON_MASK     0b00001111
#define LED_COMMON_BIT(pos) (3 - (pos))
#define LED_SEGMENT_PORT    P3

#endif  // board_yellow_small

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#if BOARD_GREEN_SMALL

#define DP1_IS_COLON TRUE

// Pushbutton port pins
#define S2 P1_3
#define S1 P1_4

// DS1302 pin to port mapping
#if PROC_IS_15W404AS
    #define DS1302_CE_SBIT      P5_5
    #define DS1302_IO_SBIT      P3_1
    #define DS1302_SCK_SBIT     P3_2
#elif PROC_IS_15F204EA
    #define DS1302_CE_SBIT      P1_0
    #define DS1302_IO_SBIT      P3_1
    #define DS1302_SCK_SBIT     P3_2
#endif

// adc channels for sensors
#define ADC_LDR   4
#define ADC_TEMP  3

// buzzer port pins and active state set
#define BUZZ_SBIT   P3_0
#define BUZZ_ON     0

#define LED_COMMON_PORT     P3
#define LED_COMMON_MASK     0b01111000
#define LED_COMMON_BIT(pos) (3 + (pos)
#define LED_SEGMENT_PORT    P2

#endif  // board_green_small

#endif  // file
