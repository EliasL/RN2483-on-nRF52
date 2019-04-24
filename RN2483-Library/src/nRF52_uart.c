#include "nRF52_uart.h"

void nRF52_UART_claim_pins(){

  // Select TX and RX pins
  // Defined in nRF52_board_config.h
  NRF_UARTE0->PSEL.TXD = nRF52_PIN_TXD;
  NRF_UARTE0->PSEL.RXD = nRF52_PIN_RXD;

  NRF_GPIO->PIN_CNF[nRF52_PIN_RXD] = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
  //NRF_GPIO->PIN_CNF[nRF52_PIN_RXD] = (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos); // Don't know if works
}

void nRF52_UART_init(){
        // Configure the UARTE with no flow control and 57600 baud rate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos); 
  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud57600 << UARTE_BAUDRATE_BAUDRATE_Pos;
  // Enable the UART (starts using the TX/RX pins)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
  nRF52_UART_claim_pins();

  //Enable detect
  NRF_GPIOTE->INTENSET = (GPIOTE_INTENSET_PORT_Enabled << GPIOTE_INTENSET_PORT_Pos);
  // Enable interrupts for GPIOTE
  NVIC_EnableIRQ(GPIOTE_IRQn);
}

void nRF52_UART_release_all_pins(){
  // Stop the UARTE
  NRF_UARTE0->TASKS_STOPTX = 1;
  // Wait until we receive the stopped event
  while (NRF_UARTE0->EVENTS_TXSTOPPED == 0);
  
  // Disable the UARTE (pins are now available for other use)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
}

void nRF52_UART_quit(){
  nRF52_UART_release_all_pins();
}

void nRF52_UART_autobaud(){
  /*
  See 1.4 UART INTERFACE p.12
  http://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf
  */

  const uint8_t string[3] = "U\r\n"; // char U is 0x55 or 01010101 in binary
  uint16_t STRING_LENGTH = 3;
  uint8_t ram_string[STRING_LENGTH];
  string_memory_location_manipulation(string, ram_string, STRING_LENGTH);

 // Keep pin low to trigger break condition
 nRF52_UART_send_long_0x00();

 // Send message
  NRF_UARTE0->EVENTS_ENDTX = 0;
  NRF_UARTE0->TXD.MAXCNT = STRING_LENGTH;
  NRF_UARTE0->TXD.PTR = (uint32_t)ram_string;
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);
}

void nRF52_UART_send_long_0x00(){
  // While sending a few 0x00 chars would be a lot simpler, it doesn't work because of spacer bits between each char
  // So the idea is to use a very slow baudrate, and send one 0x00.

  // Configure the UARTE with no flow control and 1200 baud rate, the slowest baudrate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos); 
  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud1200 << UARTE_BAUDRATE_BAUDRATE_Pos;

  // Send 0x00
  NRF_UARTE0->EVENTS_ENDTX = 0;
  NRF_UARTE0->TXD.MAXCNT = 1;
  NRF_UARTE0->TXD.PTR = (uint32_t)0x00;
  NRF_UARTE0->TASKS_STARTTX = 1;
  
  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);

  //return to normal baudrate
  nRF52_UART_init();
}

uint8_t nRF52_UART_write(const uint8_t * string){

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


uint8_t stringStorage[255] = {0};
bool doneReading = false;
int responceLength = 0;

ResponceFunction currentResponceFunc;


extern void nRF52_UART_read(ResponceFunction handler){
  // The general idea will be to assume that stringStorage will never be filled, and to just wait for a timeout event
  
  // Clear the string storage
  // We really only need to remove any \n, but we might as well just clear everything
  memset(stringStorage,0,sizeof(stringStorage));
  doneReading = false;
  NRF_UARTE0->EVENTS_ENDRX = 0;

  // Configure transmit buffer and start listening
  NRF_UARTE0->EVENTS_ENDRX = 0;
  NRF_UARTE0->RXD.PTR = (uint32_t)&stringStorage[0];
  NRF_UARTE0->RXD.MAXCNT = sizeof(stringStorage);
  NRF_UARTE0->TASKS_STARTRX = 1;
  currentResponceFunc = handler;
}

void nRF53_UART_handle_ENDRX_event(){
  if(currentResponceFunc != 0){
    currentResponceFunc(stringStorage, NRF_UARTE0->RXD.AMOUNT);
  }
  responceLength = NRF_UARTE0->RXD.AMOUNT;
}

void nRF52_UART_handle_RXDRDY_event(){
  // Search for \r
  // For some reason, it doesn't work to search for \n
  if(string_contains_char(stringStorage, sizeof(stringStorage), '\r')){
    // The responce is complete
    NRF_UARTE0->TASKS_STOPRX = 1;
      
    doneReading = true;
    //We wait for the end event
    while (!NRF_UARTE0->EVENTS_ENDRX){}
    nRF53_UART_handle_ENDRX_event();
  
  }
  NRF_UARTE0->EVENTS_RXDRDY = 0;
}

void GPIOTE_IRQHandler(void)
{
  // This handler will be run after wakeup from system ON (GPIO wakeup)
  if(NRF_GPIOTE->EVENTS_PORT){

    NRF_GPIOTE->EVENTS_PORT = 0;

    if(NRF_UARTE0->EVENTS_RXDRDY){
      nRF52_UART_handle_RXDRDY_event();
    }
  }
}

void waitForResponce(){
  while (!NRF_UARTE0->EVENTS_ENDRX){}
}

void standardResponceHandler(uint8_t *responce, int length){
    debug_print("Responce: %s", responce);
}

int nRF52_UART_blocking_read(uint8_t* buffer){
  nRF52_UART_read(NULL);
  waitForResponce();
  strcpy((char * restrict)buffer,(char * restrict)stringStorage);
  return responceLength;
}