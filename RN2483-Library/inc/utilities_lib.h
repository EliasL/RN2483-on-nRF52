#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <nrf.h>
#include <nrf_delay.h>

/*
* Function: length_of_string
* ----------------------------
*  Iterates through string and searches for \0 char
* 
*  returns length of string
*/
int length_of_string(const uint8_t *string);


/*
* Function: string_memory_location_manipulation
* ----------------------------
*  Changes the location of a variable from flash memory to RAM
* 
*  source: A pointer that might be in flash memory
*  destination: A pointer that the function makes sure is pointing to RAM
*  STRING_LENGTH: length of string in source
*/
void string_memory_location_manipulation(const uint8_t * source, uint8_t * destination, int STRING_LENGTH);


/*
* Function: stringContains
* ----------------------------
*  Checks if a string contains another string
* 
*  whole: A string that might contain piece
*  piece: A string
*  return: true if piece is in whole, if not, false
*/
bool string_contains(uint8_t * whole, uint8_t * piece);

/*
* Function: wait_a_bit
* ----------------------------
*  Runs a long loop, previously very simply, but now with nrf_delay.h
*
*  seconds: Number of seconds to wait
*/
void wait_a_bit(float seconds);

/*
* Function: led_init
* ----------------------------
*  Initialize pin for driving a led diode
*
*  led_gpio_pin: The pin connected to the led
*/
void led_init(const unsigned long led_gpio_pin);

/*
* Function: led_on
* ----------------------------
*  Turn a led on
*
*  led_gpio_pin: The pin connected to the led
*/
void led_on(const unsigned long led_gpio_pin);

/*
* Function: led_off
* ----------------------------
*  Turn a led off
*
*  led_gpio_pin: The pin connected to the led
*/
void led_off(const unsigned long led_gpio_pin);

/*
* Function: led_toggle
* ----------------------------
*  Toggle a led
*
*  led_gpio_pin: The pin connected to the led
*/
void led_toggle(const unsigned long led_gpio_pin);


/*
* Function: button_init
* ----------------------------
*  Initialize pin for handeling a button
*
*  button_gpio_pin: The pin connected to the button
*/
void button_init(const unsigned long button_gpio_pin);

/*
* Function: button_is_pressed
* ----------------------------
*  Returns true if button is pressed
*
*  button_gpio_pin: The pin connected to the button
*/
bool button_is_pressed(const unsigned long button_gpio_pin);

/*
* Function: init_button_check_timer
* ----------------------------
*  Initializes so that TIMER0_IRQHandler will be called every 10 ms
*
*/
void init_button_check_timer();