#include <nrf.h>


/*
* Function: nRF52_pin_init
* ----------------------------
*   Configure GPIO pin as output with standard drive strength.
*   PIN_GPIO: pin to be configured
*/
void nRF52_pin_init(unsigned long PIN_GPIO);


/*
* Function: nRF52_pin_write
* ----------------------------
*   Set pin High if value does not equal 0, Low if value is 0
*   PIN_GPIO: pin to be written to
*   value: value to be interpreted
*/
void nRF52_pin_write(unsigned long PIN_GPIO, uint8_t value);