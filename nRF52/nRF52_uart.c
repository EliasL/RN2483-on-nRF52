#include "nRF52_uart.h"

#define nRF52_RESET_PIN_FOR_RN2483 11
#define PIN_HIGH 1
#define PIN_LOW 0


void nRF52_uart_init(uint8_t PIN_TXD, uint8_t PIN_RXD){
      // Configure the UARTE with no flow control, one parity bit and 115200 baud rate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Included << UART_CONFIG_PARITY_Pos); 
  
  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200 << UARTE_BAUDRATE_BAUDRATE_Pos;

  nRF52_uart_claim_pins(PIN_TXD, PIN_RXD);
}

uint8_t nRF52_uart_write(uint8_t *string){
  uint16_t STRING_LENGTH = length_of_string(string);
  // If \0 was not found
  if(STRING_LENGTH == 0){
     return 1;
  }

  // Configure transmit buffer and start the transfer
  NRF_UARTE0->TXD.MAXCNT = STRING_LENGTH;
  NRF_UARTE0->TXD.PTR = (uint32_t)&string[0];
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);
  return 0;
}

uint8_t nRF52_uart_read(){
  // https://devzone.nordicsemi.com/f/nordic-q-a/33813/nrf58232-uarte-rx-easydma-pointer

  static uint8_t rxUarteBuffer[1]={0};
  // Configure transmit buffer and start listening
  NRF_UARTE0->SHORTS = (UARTE_SHORTS_ENDRX_STARTRX_Enabled << UARTE_SHORTS_ENDRX_STARTRX_Pos);
  NRF_UARTE0->RXD.PTR = (uint32_t)((uint8_t *) rxUarteBuffer);
  NRF_UARTE0->RXD.MAXCNT = sizeof(rxUarteBuffer);
  NRF_UARTE0->TASKS_STARTRX = 1;

  // Wait until the reception is complete
  //uint8_t test[] = "t";
  while (NRF_UARTE0->EVENTS_ENDRX == 0){//nRF52_uart_write(test);
  };
  return rxUarteBuffer[0];
}

void nRF52_uart_quit(){
  nRF52_uart_release_all_pins();
}

void nRF52_uart_claim_pins(uint8_t PIN_TXD, uint8_t PIN_RXD){

  // Select TX and RX pins
  NRF_UARTE0->PSEL.TXD = PIN_TXD;
  NRF_UARTE0->PSEL.RXD = PIN_RXD;

  // Enable the UART (starts using the TX/RX pins)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
}

void nRF52_uart_release_all_pins(){
  // Stop the UART TX
  NRF_UARTE0->TASKS_STOPTX = 1;
  // Wait until we receive the stopped event
  while (NRF_UARTE0->EVENTS_TXSTOPPED == 0);
  
  // Disable the UARTE (pins are now available for other use)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
}



//Helper functions
int length_of_string(uint8_t *string){
  uint16_t MAX_STRING_SIZE = 65535;
  uint16_t STRING_LENGTH = 0;

  for(uint16_t i = 0; i < MAX_STRING_SIZE; i++)
  {
    if(string[i] == '\0'){ // If the char is '\0'
      STRING_LENGTH = i;
      break;
    }
  }
  return STRING_LENGTH;
}