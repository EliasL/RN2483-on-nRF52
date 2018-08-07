#include "nRF52/nRF52_uart.h"
#include "nRF52/nRF52_pin.h"

#define PIN_GPIO  (11UL)
#define PIN_HIGH 1
#define PIN_LOW 0
int main(void)
{
  uint8_t PIN_TXD = 6;
  uint8_t PIN_RXD = 8;
  uint8_t hello_world[] = "  <-- What you typed!\n\r";

  nRF52_uart_init(PIN_TXD, PIN_RXD);
  hello_world[0] = nRF52_uart_read();
  nRF52_uart_write(hello_world);
  nRF52_uart_quit();

  nRF52_pin_init(PIN_GPIO);
  nRF52_pin_write(PIN_GPIO, PIN_HIGH);

  while (1)
  {
    __WFE();
  }
}