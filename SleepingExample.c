/*
#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"
#include "application.h"

/*
Low power radio


int main(){

    application_init();
    application_awake_cycle();

    while(true){
    // Enter System ON sleep mode
        __WFE();
        __SEV();
        __WFE();
    }
    

    return 1;
}
*/
#include <nrf.h>
#include <Simple_debug_printf.h>
#include <string.h>     //strlen, strcmp
#include <stdint.h>     //uint8_t
#include <stdbool.h>    //bool

void (*currentEventHandler)(bool,bool);

void (*oneSecondFunc)();

void swapHandler();

void eventHandler1(bool input1, bool input2){
    if(input1){
        debug_print("Swapping...");
        oneSecondFunc = &swapHandler;
    }
    if(input2){
        debug_print("1");
    }
}

void eventHandler2(bool input1, bool input2){
    if(input1){
        debug_print("Swapping...");
        oneSecondFunc = &swapHandler;
    }
    if(input2){
        debug_print("-1");
    }
}

void swapHandler(){
    debug_print("Swapped!");
    if(currentEventHandler == eventHandler1){
        currentEventHandler = &eventHandler2;
    }else{
        currentEventHandler = &eventHandler1;
    }
}

void GPIOTE_IRQHandler(void)
{
    // This handler will be run after wakeup from system ON (GPIO wakeup)
    if(NRF_GPIOTE->EVENTS_PORT)
    {
        NRF_GPIOTE->EVENTS_PORT = 0;

        uint32_t volatile input1;
        uint32_t volatile input2;
        // Get the value set in the 13th bit and shift it to get 1 or 0
        input1 = (NRF_GPIO->IN >> 16) & 1UL;
        input2 = (NRF_GPIO->IN >> 15) & 1UL;
        currentEventHandler(input1, input2);
    }
}


void init_nRF52_Timer_RTC0(){

  // Start LFCLK (32kHz) crystal oscillator. If you don't have crystal on your board, choose RCOSC instead.
  NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

  // f_RTC [kHz] = 32.768 / (PRESCALER + 1 )
  // 1/4 second timer ? umm...
  NRF_RTC0->PRESCALER = 4095;

  //  1s compare value, generates EVENTS_COMPARE[0] after 1 seconds
  NRF_RTC0->CC[0] = 4;


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
    NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + 4;

    // Read back event register so ensure we have cleared it before exiting IRQ handler.
    dummy = NRF_RTC0->EVENTS_COMPARE[0];
    dummy;

    if(oneSecondFunc != 0){
      oneSecondFunc();
      oneSecondFunc=0;
    }

  }
} 

int main(){

    debug_print("The program has started!");

    //Enable detect
    NRF_GPIOTE->INTENSET = (GPIOTE_INTENSET_PORT_Enabled << GPIOTE_INTENSET_PORT_Pos);
    // Enable interrupts for GPIOTE
    NVIC_EnableIRQ(GPIOTE_IRQn);

    // Configure the GPIO pin for Buttons
    // as input with pull-up resistor enabled.
    NRF_GPIO->PIN_CNF[16] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                            (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                            (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                            (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
                            (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);

    NRF_GPIO->PIN_CNF[15] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                            (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                            (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                            (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
                            (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
    
    init_nRF52_Timer_RTC0();


    currentEventHandler = &eventHandler2;

    while(true){
        // Make sure any pending events are cleared
        __SEV();
        __WFE();
        // Enter System ON sleep mode
        __WFE();
    } 
}