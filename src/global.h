#ifndef _GLOBAL_H
#define _GLOBAL_H

#define TRUE    1                   // for general use, bit setters too
#define FALSE   0

//---------------------------------------------------------------------------
// Software Option configuration
//---------------------------------------------------------------------------

#define OPT_DAY_ALPHA
#define OPT_MONTH_ALPHA
#define OPT_BLANK_ZERO          // display '1:00' rather than '01:00'
#define SETUP_TIMEOUT   60      // seconds
//#define DEBUG                 // serial debugging
//#define TEST_DEFAULTS         // load defaults at every power-up

//---------------------------------------------------------------------------
// Hardware Option configuration
//---------------------------------------------------------------------------

#define FOSC    22118400L           // clock speed in Hz

// Board type definitions
#define BOARD_TALKING       1
#define BOARD_BLUE_6        2
#define BOARD_BLUE_5_RELAY  3
#define BOARD_YELLOW_5      4
#define BOARD_YELLOW_SMALL  5
#define BOARD_WHITE_SMALL   6
#define BOARD_GREEN_SMALL   7

// Processor type definitions
#define PROC_15W401AS       1       // e.g. 15W404AS, 15W408AS
#define PROC_15F204EA       2

// Select board and processor types from the above options
#define BOARD_TYPE          BOARD_YELLOW_SMALL
#define PROC_TYPE           PROC_15W401AS

#define HAS_LDR             // light-dependent resistor
//#define COMMON_ANODE      // else common cathode
//#define DIGIT_2_FLIP

//---------------------------------------------------------------------------
// Board hardware defines
//---------------------------------------------------------------------------

#ifndef BOARD_TYPE
#error BOARD_TYPE is not defined

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_TALKING

#define HAS_NY3P_SPEECH
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

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_BLUE_6

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

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_BLUE_5_RELAY

// Pushbutton port pins
#define S1 P3_0
#define S2 P3_1

// DS1302 pin to port mapping
#if PROC_TYPE == PROC_15W401AS
    #define DS1302_CE_SBIT      P5_4
    #define DS1302_IO_SBIT      P5_5
#elif PROC_TYPE == PROC_15F204EA
    #define DS1302_CE_SBIT      P0_0
    #define DS1302_IO_SBIT      P0_1
#else
    #error Target processor is unknown
#endif

#define DS1302_SCK_SBIT     P3_2

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

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_YELLOW_5

// Pushbutton port pins
#define S2 P3_0
#define S1 P3_1

// DS1302 pin to port mapping
#if PROC_TYPE == PROC_15W401AS
    #define DS1302_CE_SBIT      P5_4
    #define DS1302_IO_SBIT      P5_5
#elif PROC_TYPE == PROC_15F204EA
    #define DS1302_CE_SBIT      P0_0
    #define DS1302_IO_SBIT      P0_1
#else
    #error Target processor is unknown
#endif

#define DS1302_SCK_SBIT     P3_2

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

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_WHITE_SMALL

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

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_YELLOW_SMALL

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
#if PROC_TYPE == PROC_15W401AS
    #define BUZZ_SBIT   P5_5
#elif PROC_TYPE == PROC_15F204EA
    #define BUZZ_SBIT   P0_1
#else
    #error Target processor is unknown
#endif

#define BUZZ_ON     0

#define LED_COMMON_PORT     P2
#define LED_COMMON_MASK     0b00001111
#define LED_COMMON_BIT(pos) (3 - (pos))
#define LED_SEGMENT_PORT    P3

//---------------------------------------------------------------------------

#elif BOARD_TYPE == BOARD_GREEN_SMALL

// Pushbutton port pins
#define S2 P1_3
#define S1 P1_4

// DS1302 pin to port mapping
#if PROC_TYPE == PROC_15W401AS
    #define DS1302_CE_SBIT      P5_5
#elif PROC_TYPE == PROC_15F204EA
    #define DS1302_CE_SBIT      P0_1
#else
    #error Target processor is unknown
#endif

#define DS1302_IO_SBIT      P3_1
#define DS1302_SCK_SBIT     P3_2

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

//---------------------------------------------------------------------------

#else
    #error BOARD_TYPE not recognized
#endif

#endif  // file
