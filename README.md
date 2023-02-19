# Firmware replacement for Chinese STC based processor DIY Clock kits

This is a replacement program for the STC 8051 microcontroller based DIY digital clock kits available from numerous Chinese sources. It is configurable for minor hardware variations, such as MCU pin assignments. The code easily fits into a 4K microcontroller, even with the serial debug feature enabled.

This is a heavily modified fork of R. Sloyer's [STC LED Clock](https://github.com/aFewBits/stc-led-clock) project. If my version doesn't suit your needs, try his.

This version was developed using SDCC 4.0. It generates efficient code in most cases, but there are situations where it produces sub-optimal results, or is unexpectedly sensitive to source code style.

The speech feature has been removed, because it would require modifications to fit my other changes, and I have no access to a clock with a sound chip for testing.

## License

This project is licensed under the MIT License; see the `license` file for details.

## Disclaimer

This software and documentation is provided as-is, with no warranty. You may use it at your own risk. See the license for further details.

## Clock Features

* Time display (12 and 24 hour modes)
* Alarm and hourly chime
* Date and weekday display
* Ambient Temperature display, in &deg;C or &deg;F
* Configurable automatic brightness
* Temperature offset adjustment
* Auto-increment when setting times/alarms/etc. No need to repeatedly press the same key.
* Configuration menu with text prompts, within the limits of four 7-segment digits.

## Switches

The switches are assigned the following functions:

* S1: Advance to next item
* S2: Accept or increment

## Clock Display Cycling

In clock mode, press S2 once to automatically cycle through the following:

1. Weekday
2. Date
3. Temperature
4. Seconds

Each of the above items is displayed for a short time, then normal clock display resumes. If the automatic display cycling option is on, then this sequence begins automatically, once per minute.

Press S2 during weekday, date, or temperature display to resume normal clock display immediately.

Press S2 while seconds are displayed, to continue displaying seconds until S2 is pressed again.

## Menu Navigation

From clock mode:

1. Press S1 to cycle through available groups of settings.
2. Press S2 to select a group of settings.
3. Press S2 to change a setting, or S1 to advance to the next setting.
4. At the last setting in the group, press S1 to save changes and resume normal clock display.

The settings are:

1. `CL` clock settings
    1. hour
    2. minute
    3. seconds (S2 snaps to the nearest minute)
    4. `co` daily correction of -9 to +9 seconds
2. `AL` alarm settings
    1. on/off
    2. hour
    3. minute
3. `CH` chime settings
    1. on/off
    2. `Cb` chime begin hour
    3. `CE` chime end hour
4. `dAtE` date settings
    1. `Yr` year (last 2 digits only)
    2. month
    3. date (day of month)
    4. weekday
5. `CFg` configuration
    1. `12H` or `24H`
    2. `dEgC` or `dEgF`
    3. `dE31` (month/day) or `31dE` (day/month) format
    4. `AC` automatic display cycling on/off
6. `brt`
    1. `bH` brightness high limit
    2. `bL` brightness low limit
7. `CAL` temperature calibration
    1. temperature measurement offset

## Hardware Requirements

This firmware is applicable to clock kits using an STC15 series microcontroller. If your kit uses a different microcontroller, it's probably best to look for a different project that is applicable to your hardware.

Hopefully, you installed the 4-pin programming header (P1) while building your clock. If not, you may need to either desolder the 7-segment display, or solder a header (perhaps via flywires) to the board surface.

Since the original firmware loaded on the STC processor cannot be copied, you cannot save and restore what you originally received with your clock. If you want to retain this code for any reason, you must leave your original microcontroller unmodified, and purchase a compatible replacement chip. These are readily available on AliExpress and eBay in small quantities.

For connection to the board, use a USB serial adapter with 5V signals. These are commonly based on FT232, PL2303 or CH340 chipsets. It will be connected to the board as follows:

| P1 header | Serial adapter |
|-----------|----------------|
| 5V        | 5V             |
| GND       | GND            |
| P3.0      | TXD            |
| P3.1      | RXD            |

Simple direct wiring is acceptable. For development work, a more complex cable may be desirable; look in the STC datasheet for "Application Circuit Diagram for ISP".

If you intend to use the serial debug output feature, you will likely want a second USB serial adapter, ideally equipped with mini-grabbers for connecting ground and data pins to the board. Use a multimeter and/or the schematic diagram to locate a convenient ground connection. Connect your serial RXD pin to the microcontroller TXD pin (typically P3.7); see `serial.c` and the microcontroller datasheet for details. No connection of serial TXD to microcontroller RXD is necessary, as the code does not support receiving data.

## Software Requirements

To compile the software, you'll need `make` and [SDCC](http://sdcc.sf.net).

To configure the processor and flash the firmware, you'll need either [STCGAL](https://github.com/grigorig/stcgal) or [STC-ISP](http://www.stcmcudata.com/STCISP/stc-isp-15xx-v6.88R.zip) (Windows-only).

## Installation

Configure the software for your hardware and preferences by editing `global.h`.

Run `make` to compile the software; this produces `clock.hex`.

The included `program.sh` script provides an example of `stcgal` invocation.

### Programming with STC-ISP

The STC-ISP tool is pretty straightforward. The process is:

* Select the correct processor type
* Select COM port
* Open code file (main.hex)
* Select clock speed of 22.1184 MHz on the Hardware Option tab
* Click the Download/Program button
* Briefly interrupt the 5V supply to the STC processor

If you're using simple wiring, then the last step can be accomplished by unplugging the programming cable from the board, then reattaching it. Cycling the USB connection is not recommended.

If you've built the ISP cable described in the STC datasheet, use its power switch.

## Testing Changes

The `test` folder contains a very minimal (incomplete) test suite, which utilizes the `ucsim` microcontroller simulation tool that is typically included with SDCC. Simply run `make` to run the tests. If you make code changes, then I encourage you to write additional tests.

## Debugging

The code includes a simple serial output mechanism, which can be useful for debugging; see `serial.c` and `serial.h`. The simple "Print" functions provided here are sufficient for most debugging tasks. SDCC also supports a small-footprint `printf_tiny` function that only requires about 400 additional bytes of flash.

Interrupt service routines are sensitive to timing, and should not attempt to produce serial output.

## Timing

The CPU clock is set to run at 22.1184 MHz. This divides evenly by 192 to produce the standard baud rate of 115200 bps, which is used by the debug serial port.

Timer 0 is configured to tick every 50 &micro;s (20 KHz). This is the display scan rate; see `display.h`.

## Development Plans

None at this time. If you find a problem, I may be interested in trying to address it. If you want an additional feature, you're welcome to ask, but don't be surprised if I decline. You're welcome to fork the project and modify the code to suit your needs/preferences.

## Authors

* **Graham Knap** - *Major modifications*
* **R. Sloyer** - *Initial work* - [aFewBits](https://github.com/aFewBits)

## Acknowledgments

* [zerog2k](https://github.com/zerog2k) for his original STC DIY Clock work
* STC and zerog2k for the ADC code used in the LDR and temperature measurement code
* Maxim Integrated for their DS1302 app note and [8051 example code](https://www.maximintegrated.com/en/app-notes/index.mvp/id/3449)
* All the contributors to the SDCC tool set

### References

Main STC website (in Chinese):
http://www.stcmcu.com or http://www.gxwmcu.com

STC15 series English datasheet:
http://www.stcmcu.com/datasheet/stc/STC-AD-PDF/STC15-English.pdf

Older STC15F204EA English datasheet:
http://gxwmcu.com/datasheet/stc/STC-AD-PDF/STC15F204EA-series-english.pdf

Other STC documents (English):
http://stcmicro.com/sjsc.html

SDCC User Guide (PDF):
http://sdcc.sourceforge.net/doc/sdccman.pdf

Maxim DS1302 datasheet:
http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
