# RN2483 Library for nRF52
An embedded C library for the Micrchip RN2483 LoRa module using a NordicSemiconductor nRF52 platform.

## Contents
1. Overview
2. Usage
3. Comments on development
4. Authors

## Overview
This is an embedded C library for interacting with the [Microchip RN2483](https://www.microchip.com/wwwproducts/en/RN2483) LoRa module.

The library is a modified version of a [general non-platform-specific library](https://github.com/GeaRSiX/RN2483-Library) written by Alexander Collins.

The library is dependant on nRF5_SDK_15.0.0_a53641a, but to a minimal degree. It also includes support for SEGGER_RTT debugging.

A big thanks to Anders Nore for his [NordicSnippets](https://github.com/andenore/NordicSnippets).

## Usage

Make sure to remove .example from the congifg.example.h file, and add your LoRaWAN settings

Edit the the Makefile in armgcc. You need to change the relative path of xDK_TOP so that it matches where you have your SDK.

Edit nRF52_board_config.h to fit the connections on your hardware.

Run the Makefile in armgcc.

## Comments on development

The usefulness of the NordicSnippets by Anders Nore cannot be exaggerated.

The RN2483 is often used with an arduino, commonly through a [XBee Shield](http://wiki.seeedstudio.com/XBee_Shield_V2.0/). The shield includes signal gate which only opens for certain voltages. The UART of an arduino is around 5V, whereas the nRF52 uses a 3.3V UART. The gate in the shield stops some the nRF52's signals in a manner I admittedly do not quite understand, but in one way or another, the signal is disturbed. We decided to create our own shield in order to bypass the gate. 

When sending a command from the nRF52, some pseudo code could look like this:
```C
sendCommand("Command");
```
For some reason, the memory location of the string "Command" is in FLASH, instead of RAM. The performance is not the issue here, but it just so happens that the nRF52 UART does not have access to the FLASH memory. To bypass this, you could store the string in a variable first, and then pass the variable in the command function:
```C
char string[] = "Command";
sendCommand(string);
```
However, instead of doing that, we ended up using memcpy from string.h to create a new string that luckily was placed in ram.

## Authors
- Elias Lundheim (elias@tradlosetrondheim.no)
- Alexander Collins (alexander-collins@outlook.com)
- Anders Nore (https://github.com/andenore)
