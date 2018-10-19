#include "nRF52_uart.h"

void nRF52_uart_init(){
        // Configure the UARTE with no flow control and 57600 baud rate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos); 
  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud57600 << UARTE_BAUDRATE_BAUDRATE_Pos;
  nRF52_uart_claim_pins();
}

void nRF52_uart_autobaud(){
  /*
  See 1.4 UART INTERFACE p.12
  http://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf
  */

  const uint8_t string[3] = "U\r\n"; // char U is 0x55 or 01010101 in binary
  uint16_t STRING_LENGTH = 3;
  uint8_t ram_string[STRING_LENGTH];
  string_memory_location_manipulation(string, ram_string, STRING_LENGTH);

 // Keep pin low to trigger break condition
 nRF52_uart_long_0x00();

 // Send message
  NRF_UARTE0->EVENTS_ENDTX = 0;
  NRF_UARTE0->TXD.MAXCNT = STRING_LENGTH;
  NRF_UARTE0->TXD.PTR = (uint32_t)ram_string;
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);
}

void nRF52_uart_long_0x00(){

  // While sending a few 0x00 chars would be a lot simpler, it doesn't work because of the spacer bits between each char
  // So the idea is to use a very slow baudrate, and send one 0x00.

  // Configure the UARTE with no flow control and 1200 baud rate, the slowest baudrate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos); 
  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud1200 << UARTE_BAUDRATE_BAUDRATE_Pos;
  nRF52_uart_claim_pins();

  // Send 0x00
  NRF_UARTE0->EVENTS_ENDTX = 0;
  NRF_UARTE0->TXD.MAXCNT = 1;
  NRF_UARTE0->TXD.PTR = (uint32_t)0x00;
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);

  //return to normal baudrate
  nRF52_uart_init();
}

uint8_t nRF52_uart_write(const uint8_t * string){

  uint16_t STRING_LENGTH = length_of_string(string);
  // If \0 was not found
  if(STRING_LENGTH == 0){
     return 1;
  }
  uint8_t ram_string[STRING_LENGTH];
  string_memory_location_manipulation(string, ram_string, STRING_LENGTH);
  

  // Configure transmit buffer and start the transfer
  NRF_UARTE0->EVENTS_ENDTX = 0;
  NRF_UARTE0->TXD.MAXCNT = STRING_LENGTH;
  NRF_UARTE0->TXD.PTR = (uint32_t)ram_string;
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);
  return 0;
}

int nRF52_uart_read(uint8_t * buffer, int length, int extraWaitTime){
  //The RN2483 sometimes uses a long time to perform certain opperations, therefore,
  //the uart will time out many times trying to read a response. If that is the case, the nRF52_uart_readSB() sends ' '
  //as the read char. We can handle this here by removing any ' ' ONLY at the beginning of a string
  //We will of course not change any ' ' within a message, but only at the beginning.

  int maxSpaceChars=100*extraWaitTime;
  int beginningSpaceCounter = 0;

  for(int i=0; i<length;i++){
    buffer[i] = nRF52_uart_readSB();
    //Check for the end of the message
    if(buffer[i]=='\n'){
      //Add a null char in case the string we are using isn't empty from before
      buffer[i+1]='\0';
      return i;
      }

    //Handle timeout chars
    if(buffer[0] == ' '){ //Note it only checks buffer[0], not buffer[i]
      i--;
      beginningSpaceCounter++;
      if(beginningSpaceCounter >= maxSpaceChars){
        debug_print("Timeout!");
        return 0;
      }
    }
  }
  return 0;
}

uint8_t nRF52_uart_readSB(){
  uint8_t byte[1] = {' '}; //Send ' ' if no byte is received

  // Configure transmit buffer and start listening
  NRF_UARTE0->EVENTS_ENDRX = 0;
  NRF_UARTE0->SHORTS = (UARTE_SHORTS_ENDRX_STARTRX_Enabled << UARTE_SHORTS_ENDRX_STARTRX_Pos);
  NRF_UARTE0->RXD.PTR = (uint32_t)((uint8_t *) byte);
  NRF_UARTE0->RXD.MAXCNT = 1;
  NRF_UARTE0->TASKS_STARTRX = 1;

  int timeout=100000;
  int timer=0;
  while (NRF_UARTE0->EVENTS_ENDRX == 0 && timer<timeout){timer++;};

  //Reset ENDRX event
  NRF_UARTE0->EVENTS_ENDRX = 0;
  return byte[0];
}

void nRF52_uart_quit(){
  nRF52_uart_release_all_pins();
}

void nRF52_uart_claim_pins(){

  // Select TX and RX pins
  // Defined in nRF52_board_config.h
  NRF_UARTE0->PSEL.TXD = nRF52_PIN_TXD;
  NRF_UARTE0->PSEL.RXD = nRF52_PIN_RXD;

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
