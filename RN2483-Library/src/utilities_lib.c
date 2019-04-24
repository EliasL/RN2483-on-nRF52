#include "utilities_lib.h"

void (*oneSecondFunc)();

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

void wait_a_bit(float seconds){
  //Don't use this function
  int ms = (int)(seconds * 1000);
  nrf_delay_ms(ms);
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
bool string_contains_char(uint8_t *string, int length, char piece){
  for(int i = 0; i<length; i++){
    if(string[i] == piece){
      return true;
    }
  }
  return false;
}

void init_nRF52_Timer_RTC0(){

  // Start LFCLK (32kHz) crystal oscillator. If you don't have crystal on your board, choose RCOSC instead.
  NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

  // f_RTC [kHz] = 32.768 / (PRESCALER + 1 )
  // 1 second timer
  NRF_RTC0->PRESCALER = 32769;

  //  1s compare value, generates EVENTS_COMPARE[0] after one second second
  NRF_RTC0->CC[0] = 1;


  // Enable EVENTS_COMPARE[0] generation
  NRF_RTC0->EVTENSET = RTC_EVTENSET_COMPARE0_Enabled << RTC_EVTENSET_COMPARE0_Pos;
  // Enable IRQ on EVENTS_COMPARE[0]
  NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Enabled << RTC_INTENSET_COMPARE0_Pos;

  // Enable RTC IRQ
  NVIC_EnableIRQ(RTC0_IRQn);
  NRF_RTC0->TASKS_START = 1;
}

void RTC0_IRQHandler(void){
  volatile uint32_t dummy;
  if (NRF_RTC0->EVENTS_COMPARE[0] == 1)
  {
    NRF_RTC0->EVENTS_COMPARE[0] = 0;

    // Add to counter so event is called again
    NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + 1;

    // Read back event register so ensure we have cleared it before exiting IRQ handler.
    dummy = NRF_RTC0->EVENTS_COMPARE[0];
    dummy;

    if(oneSecondFunc != 0){
      oneSecondFunc();
      oneSecondFunc = 0;
    }

  }
} 

void led_init(const unsigned long led_gpio_pin){
  // Configure the GPIO pin for LED 1 on the nRF52832 dev kit
  // as output with standard drive strength.
  NRF_GPIO->PIN_CNF[led_gpio_pin] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                                   (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
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
                                  (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos); //Don't know if works
}

bool button_is_pressed( const unsigned long button_gpio_pin){
	// Get the value set in the "pin2-th bit and shift it to get 1 or 0
  return !((NRF_GPIO->IN >> button_gpio_pin) & 1UL);
}

int32_t get_temperature(){
  
  volatile int32_t temperature;

  // Start HFCLK crystal oscillator, since that will give highest accuracy
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

  // Trigger temperature measurement
  NRF_TEMP->TASKS_START = 1;

  // Wait until measurement is finished
  while (NRF_TEMP->EVENTS_DATARDY == 0);
  NRF_TEMP->EVENTS_DATARDY = 0;
  NRF_CLOCK->TASKS_HFCLKSTOP = 1;
  // Read temperature and convert to celcius (rounding down)
  temperature = (int32_t)NRF_TEMP->TEMP / 4;
  return temperature;
}