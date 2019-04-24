#include <nrf.h>
#include <string.h>
#include <Simple_debug_printf.h>
#include <stdio.h>
#include "nRF52_board_config.h"
#include "utilities_lib.h"

/*
* Function: nRF52_UART_init
*
*   Configure the UARTE with no flow control, and 57600 baud rate
*   and enables the UART to start using the TX/RX pins nRF52_PIN_TXD and nRF52_PIN_RXD
*   defined in nRF52_board_config.h
*/
void nRF52_UART_init();


/*
* Function: nRF52_UART_claim_pins
*
*   Enables the UART to start using the TX/RX pins
*
*   Uses pins defined in nRF52_board_config
*/
void nRF52_UART_claim_pins();


/*
* Function: nRF52_UART_release_all_pins
*
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_UART_release_all_pins();


/*
* Function: nRF52_UART_quit
*
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_UART_quit();


/*
* Function: nRF52_UART_autobaud
*
*  Set the UART TDX pin low, followed by a 'U' char (0x55 in hex and 01010101 in binary)
* 
*/
void nRF52_UART_autobaud();

/*
* Function: nRF52_UART_write
*
*  Configure transmit buffer and start the transfer
* 
* string: A pointer to element 0 in a uint8_t array
* return: 0 for success, 1 for too long string
*/
uint8_t nRF52_UART_write(const uint8_t *string);

/*
* FunctionType: ResponceFunction
*
*   message: string of max length 255
*   length: length of message received
*/
typedef void (*ResponceFunction)(uint8_t *message, int length);


/*
* Function: nRF52_UART_read
*
*  Begins a read and immediately returns
* 
*  handler: the next function of the program that wants to read
*/
void nRF52_UART_read(ResponceFunction handler);

/*
* Function: nRF52_UART_blocking_read
*
*  Starts a read, and waits for it to complete
* 
*  buffer: The buffer receiving the responce
*  return: Returns the length of the responce
*/
int nRF52_UART_blocking_read(uint8_t *buffer);

/*
* Function: standardResponceHandler
*
*  Prints out response
* 
*  responce: The buffer receiving the responce
*  length: Length of the responce
*/
void standardResponceHandler(uint8_t *responce, int length);

/*
* Function: nRF52_UART_long_0x00
*
*  Sets baudrate to 1200, and sends one 0x00
* 
*/
void nRF52_UART_send_long_0x00();

