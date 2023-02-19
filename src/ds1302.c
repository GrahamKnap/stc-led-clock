// DS1302 pin mappings must be defined in "global.h".
// For each of CE/SCK/IO:
//
// * If the pin is addressable by sbit (typical):
//
//   #define DS1302_xx_SBIT P1_0
//
// * Otherwise, if it's only addressable by byte sfr (rare):
//
//   #define DS1302_xx_SFR P5
//   #define DS1302_xx_BIT 5

#include "global.h"

// definitions for this file
#include "ds1302.h"

// other local headers
// "config.h" is expected to declare a structure variable called "config".
#include "config.h"

// system headers
#include <stddef.h>
#include "stc15.h"

// project globals
CLOCK clock;


// DS1302 command byte bit assignments:
//   7: always 1
//   6: 0 = clock/calendar, 1 = RAM
// 5-1: address
//   0: 0 = write, 1 = read
#define DS1302_CMD(ram_clk, addr, rd_wr) \
    (0x80 | ((ram_clk) ? 0x40 : 0) | ((uint8_t)(addr) << 1) | ((rd_wr) ? 0x01 : 0))

// The high-order bit is transmitted last, and is always 1.
// Writing 1 to an 8051 quasi-bidirectional I/O pin enables a weak pull-up,
// allowing the pin to function as an input.

#define DS1302_CLK      0
#define DS1302_RAM      1

#define DS1302_SECOND   0
#define DS1302_MINUTE   1
#define DS1302_HOUR     2
#define DS1302_DATE     3
#define DS1302_MONTH    4
#define DS1302_WEEKDAY  5
#define DS1302_YEAR     6
#define DS1302_CONTROL  7
#define DS1302_TRICKLE  8
#define DS1302_BURST    31

#define DS1302_WRITE    0
#define DS1302_READ     1


// Macros for accessing CE/SCK/IO pins
#define CONCAT(a,b)         a ## b
#define ASM_NAME(name)      CONCAT(_,name)

#ifdef DS1302_CE_SBIT
    #define ASM_CE_LO       clr ASM_NAME(DS1302_CE_SBIT)
    #define ASM_CE_HI       setb ASM_NAME(DS1302_CE_SBIT)
#elif defined(DS1302_CE_SFR) && defined(DS1302_CE_BIT)
    #define CE_BIT_VAL      (1 << DS1302_CE_BIT)
    #define ASM_CE_LO       anl ASM_NAME(DS1302_CE_SFR),#(~CE_BIT_VAL)
    #define ASM_CE_HI       orl ASM_NAME(DS1302_CE_SFR),#(CE_BIT_VAL)
#else
    #error DS1302 CE pin not defined
#endif

#ifdef DS1302_SCK_SBIT
    #define ASM_SCK_LO      clr ASM_NAME(DS1302_SCK_SBIT)
    #define ASM_SCK_HI      setb ASM_NAME(DS1302_SCK_SBIT)
#elif defined(DS1302_SCK_SFR) && defined(DS1302_SCK_BIT)
    #define SCK_BIT_VAL     (1 << DS1302_SCK_BIT)
    #define ASM_SCK_LO      anl ASM_NAME(DS1302_SCK_SFR),#(~SCK_BIT_VAL)
    #define ASM_SCK_HI      orl ASM_NAME(DS1302_SCK_SFR),#(SCK_BIT_VAL)
#else
    #error DS1302 SCK pin not defined
#endif

#ifdef DS1302_IO_SBIT
    #define ASM_IO_SBIT     ASM_NAME(DS1302_IO_SBIT)
#elif defined(DS1302_IO_SFR) && defined(DS1302_IO_BIT)
    #define ASM_IO_SFR      ASM_NAME(DS1302_IO_SFR)
    #define IO_BIT_VAL      (1 << DS1302_IO_BIT)
#else
    #error DS1302 IO pin not defined
#endif


static void BeginComm(void) __naked
{
    __asm
    ASM_SCK_LO
    ASM_CE_LO
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7
    ASM_CE_HI
    ret
    __endasm;
}

static void EndComm(void) __naked
{
    __asm
    ASM_SCK_LO
    ASM_CE_LO
    ret
    __endasm;
}

static void SendByte(uint8_t x) __naked
{
    (void)x;            // avoid "unused parameter" warning

    __asm
    push ar6            // save registers
    mov r6,#8           // loop counter
    mov a,dpl           // data to send

#ifdef DS1302_IO_SBIT
SendLoop:
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7
    ASM_SCK_LO          // 3T
    rrc a               // 1T discard low bit into carry
    mov ASM_IO_SBIT,c   // 3T output
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7
    ASM_SCK_HI          // 3T DS1302 input on rising edge
    djnz r6,SendLoop    // 4T
#elif defined(DS1302_IO_SFR) && defined(DS1302_IO_BIT)
SendLoop:
    rrc a               // 1T discard low bit into carry
    jc SendHi           // 3T
    ASM_SCK_LO          // 3T
    anl ASM_IO_SFR,#(~IO_BIT_VAL)   // 3T output lo
    sjmp SentBit        // 3T
SendHi:
    ASM_SCK_LO          // 3T
    orl ASM_IO_SFR,#(IO_BIT_VAL)    // 3T output hi
    sjmp SentBit        // 3T equalize timing
SentBit:
    nop                 // 1
    nop                 // 2
    ASM_SCK_HI          // 3T DS1302 input on rising edge
    djnz r6,SendLoop    // 4T
#else
    #error DS1302 IO pin not defined
#endif

    pop ar6             // restore registers
    ret
    __endasm;
}

static uint8_t RecvByte(void) __naked
{
    __asm

#ifdef DS1302_IO_SBIT
    mov a,#0x80         // bitwise loop-control value
RecvLoop:
    ASM_SCK_HI          // 3T DS1302 IO tristated on rising edge
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7              // 
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7              // 
    ASM_SCK_LO          // 3T DS1302 output on falling edge
    nop                 // 1
    nop                 // 2
    mov c,ASM_IO_SBIT   // 2T input
    rrc a               // 1T shift new bit in, loop-control bit out
    jnc RecvLoop        // 3T
#elif defined(DS1302_IO_SFR) && defined(DS1302_IO_BIT)
    push ar5            // save registers
    mov r5,#0x80        // bitwise loop-control value
    // "a" contains a random value; this is harmless
RecvLoop:
    ASM_SCK_HI          // 3T DS1302 IO tristated on rising edge
    add a,#(256 - IO_BIT_VAL)   // 2T copy IO bit to C
    mov a,r5            // 1T restore data received so far
    rrc a               // 1T shift new bit in, loop-control bit out
    inc r7              // 4T delay with inc/dec pair (2 bytes)
    dec r7              // 
    ASM_SCK_LO          // 3T DS1302 output on falling edge
    mov r5,a            // 1T save data received so far
    mov a,ASM_IO_SFR    // 2T input
    anl a,#(IO_BIT_VAL) // 2T isolate IO bit
    jnc RecvLoop        // 3T
    add a,#(256 - IO_BIT_VAL)   // 2T copy IO bit to C
    mov a,r5            // 1T restore data received so far
    rrc a               // 1T shift new bit in
    pop ar5             // restore registers
#else
    #error DS1302 IO pin not defined
#endif

    mov dpl,a           // return value
    ret
    __endasm;
}

static void ReadClockData(uint8_t count)
{
    uint8_t i, x;

    BeginComm();
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_BURST, DS1302_READ));

    for (i = 0; i < count; i++)
    {
        x = RecvByte();
        ((uint8_t *)&clock)[i] = x;
    }

    EndComm();
}

void ReadClock(void)
{
    ReadClockData(sizeof(CLOCK));
}

void WriteDate(void)
{
    // read the time, then write the entire clock
    ReadClockData(offsetof(CLOCK, date));
    WriteClock();
}

void WriteClock(void)
{
    uint8_t i, x;

    clock.control = 0;
    BeginComm();
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_BURST, DS1302_WRITE));

    for (i = 0; i < sizeof(CLOCK); i++)
    {
        x = ((uint8_t *)&clock)[i];
        SendByte(x);
    }

    EndComm();
}

void ReadConfig(void)
{
    uint8_t i, x;

    BeginComm();
    SendByte(DS1302_CMD(DS1302_RAM, DS1302_BURST, DS1302_READ));

    for (i = 0; i < sizeof(config); i++)
    {
        x = RecvByte();
        ((uint8_t *)&config)[i] = x;
    }

    EndComm();
}

void WriteConfig(void)
{
    uint8_t i, x;

    BeginComm();
    SendByte(DS1302_CMD(DS1302_RAM, DS1302_BURST, DS1302_WRITE));

    for (i = 0; i < sizeof(config); i++)
    {
        x = ((uint8_t *)&config)[i];
        SendByte(x);
    }

    EndComm();
}

void InitRtc(void)
{
    uint8_t t;

    BeginComm();
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_CONTROL, DS1302_WRITE));
    SendByte(0);    // disable write-protect
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_TRICKLE, DS1302_WRITE));
    SendByte(0);    // disable trickle-charge
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_SECOND, DS1302_READ));
    t = RecvByte();
    t &= 0x7f;          // mask off clock halt bit
    SendByte(DS1302_CMD(DS1302_CLK, DS1302_SECOND, DS1302_WRITE));
    SendByte(t);        // and write it back to seconds reg
    EndComm();
}
