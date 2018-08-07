#include "nRF52_pin.h"


void nRF52_pin_init(unsigned long PIN_GPIO){
// Configure GPIO pin as output with standard drive strength.
NRF_GPIO->PIN_CNF[PIN_GPIO] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                              (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}


void nRF52_pin_write(unsigned long PIN_GPIO, uint8_t value){
    if(value == 0){
         NRF_GPIO->OUTCLR = (1UL << PIN_GPIO);   
    }
    else{
          NRF_GPIO->OUTSET = (1UL << PIN_GPIO); 
    }
}