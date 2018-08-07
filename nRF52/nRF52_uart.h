#include <nrf.h>

/*
* Function: nRF52_uart_init
* ----------------------------
*   Configure the UARTE with no flow control, one parity bit and 115200 baud rate
*   and enables the UART to start using the TX/RX pins
*   PIN_TXD: Transmitt
*   PIN_RXD: Recieve
*/
void nRF52_uart_init(uint8_t PIN_TXD, uint8_t PIN_RXD);


/*
* Function: nRF52_uart_quit
* ----------------------------
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_uart_quit();


/*
* Function: nRF52_uart_write
* ----------------------------
*  Configure transmit buffer and start the transfer
* 
* string: A pointer to element 0 in a uint8_t array
* return: 0 for success, 1 for too long string
*/
uint8_t nRF52_uart_write(uint8_t *string);


/*
* Function: nRF52_uart_read
* ----------------------------
*  Read a single byte
* 
* return: One received byte
*/
uint8_t nRF52_uart_read();


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
*   PIN_TXD: Transmitt
*   PIN_RXD: Recieve
*/
void nRF52_uart_claim_pins(uint8_t PIN_TXD, uint8_t PIN_RXD);


/*
* Function: nRF52_uart_release_all_pins
* ----------------------------
*   Stop the UART TX  
*   Disable the UARTE (pins are now available for other use)
*/
void nRF52_uart_release_all_pins();