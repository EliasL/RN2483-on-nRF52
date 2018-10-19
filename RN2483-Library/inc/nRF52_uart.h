#include <nrf.h>
#include <string.h>
#include <Simple_debug_printf.h>
#include <stdio.h>
#include "nRF52_board_config.h"
#include "utilities_lib.h"

/*
* Function: nRF52_uart_init
* ----------------------------
*   Configure the UARTE with no flow control, and 57600 baud rate
*   and enables the UART to start using the TX/RX pins nRF52_PIN_TXD and nRF52_PIN_RXD
*   defined in nRF52_board_config.h
*/
void nRF52_uart_init();


/*
* Function: nRF52_uart_quit
* ----------------------------
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_uart_quit();


/*
* Function: nRF52_uart_autobaud
* ----------------------------
*  Set the uart TDX pin low, followed by a 'U' char (0x55 in hex and 01010101 in binary)
* 
*/
void nRF52_uart_autobaud();

/*
* Function: nRF52_uart_write
* ----------------------------
*  Configure transmit buffer and start the transfer
* 
* string: A pointer to element 0 in a uint8_t array
* return: 0 for success, 1 for too long string
*/
uint8_t nRF52_uart_write(const uint8_t *string);


/*
* Function: nRF52_uart_read
* ----------------------------
*  Read until \n or timeout
* 
*  buffer: output string
*  length: max length of the buffer
*  extraWaitTime: multiplies the normal timeout limit with extraWaitTime
*
*  return: length of read responce
*/
int nRF52_uart_read(uint8_t *buffer, int length, int extraWaitTime);


/*
* Function: nRF52_uart_long_0x00
* ----------------------------
*  Sets baudrate to 1200, and sends one 0x00
* 
*/
void nRF52_uart_long_0x00();


/*
* Function: nRF52_uart_readSB
* ----------------------------
*  Read a single byte
* 
*  returns the byte that was read
*/
uint8_t nRF52_uart_readSB();


/*
* Function: nRF52_uart_reset
* ----------------------------
*  Resets the nRF52
* 
* return: 0 for success, 1 for failure
*/
uint8_t nRF52_uart_reset();


/*
* Function: nRF52_uart_claim_pins
* ----------------------------
*   Enables the UART to start using the TX/RX pins
*
*   Uses pins defined in nRF52_board_config
*/
void nRF52_uart_claim_pins();


/*
* Function: nRF52_uart_release_all_pins
* ----------------------------
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_uart_release_all_pins();
