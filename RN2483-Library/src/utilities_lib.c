#include "utilities_lib.h"

int length_of_string(const uint8_t *string){
  uint16_t MAX_STRING_SIZE = 65535;
  uint16_t STRING_LENGTH = 0;
  for(uint16_t i = 0; i < MAX_STRING_SIZE; i++)
  {
    if(string[i] == '\0'){ // If the char is '\0'
      STRING_LENGTH = i;
      break;
    }
  }
  if(STRING_LENGTH == 0){
    // This means that the string is only \0, since you can't have \0 in the middle of a stirng,
    // But because it is the first char of the string that has been given, the user most likely wants to send this \0
    // so we return 1 instead of 0
    return 1;
  }

  return STRING_LENGTH;
}

void string_memory_location_manipulation(const uint8_t * source, uint8_t * destination, int STRING_LENGTH){
  // The "string" variable is a pointer that may or may not be in flash memory.
  // The nrf52's uart cannot read from flash, hence, it is sometimes
  // neccecary to copy the string from flash into RAM.
  memcpy(destination, source, STRING_LENGTH);
}

bool string_contains(uint8_t *whole, uint8_t *piece){
  int wholeLength = length_of_string(whole);
  int pieceLength = length_of_string(piece);
  for(int i = 0; i<wholeLength-pieceLength; i++){
    for(int j = 0; j<pieceLength; j++){
      if(whole[i+j] != piece[j]){
        break;
      }
      return true;
    }
  }
  return false;
}

void wait_a_bit(float bits){
  uint32_t volatile tmo;
  tmo = 10000000 * bits;
  while (tmo--);
}

void led_init(const unsigned long led_gpio_pin){
  // Configure the GPIO pin for LED 1 on the nRF52832 dev kit
  // as output with standard drive strength.
  NRF_GPIO->PIN_CNF[led_gpio_pin] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                                   (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                                   (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

void led_off(const unsigned long led_gpio_pin){
  NRF_GPIO->OUTSET = (1UL << led_gpio_pin);
}

void led_on(const unsigned long led_gpio_pin){
  NRF_GPIO->OUTCLR = (1UL << led_gpio_pin);
}

void led_toggle(const unsigned long led_gpio_pin){
  // Get the value set in the "pin"-th bit and shift it to get 1 or 0
  bool state = (NRF_GPIO->IN >> led_gpio_pin) & 1UL;
  if(state){
    led_on(led_gpio_pin);
  }
  else{
    led_off(led_gpio_pin);
  }
}

void button_init(const unsigned long button_gpio_pin){
  // Configure the GPIO pin for Button 1 on the nRF52832 dev kit
  // as input with pull-up resistor enabled.
  NRF_GPIO->PIN_CNF[button_gpio_pin] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                   (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
                                   (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

bool button_is_pressed( const unsigned long button_gpio_pin){
	// Get the value set in the "pin2-th bit and shift it to get 1 or 0
  return !((NRF_GPIO->IN >> button_gpio_pin) & 1UL);
}