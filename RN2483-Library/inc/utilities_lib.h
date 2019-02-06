#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <nrf.h>
#include <nrf_delay.h>
#include "../RTT/Simple_debug_printf.h"

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
* Function: sleep
* ----------------------------
*  Waits for event.
*/
void sleep(void);


/*
* Function: init_nRF52_Timer_RTC0
* ----------------------------
*  Prepares a very slow and inaccurate timer with the intention of having it create one event before being stopped
*
*    m = input number of minutes
*    ds = desired wait time in seconds
*    as = actual wait time in seconds
*    err = error in seconds
*
*    m   ds      as       err
*    1   60      32       28
*    2   120     96       24
*    3   180     160      20
*    8n  60*8*n  60*8*n   0
*
*    Use a multiple of 8 for best results

*/
void init_nRF52_Timer_RTC0();


/*
* Function: wake_up_after_minutes
* ----------------------------
*  Starts a timer and waits for an event
*  The event is meant to come from the timer, but any event will wake it
*  The timer is stopped in RTC0_IRQHandler
*/
void wake_up_after_minutes(unsigned int minutes);


/*
* Function: RTC0_IRQHandler
* ----------------------------
*  Stops the timer
*/
void RTC0_IRQHandler(void);


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
*  Initialize pin for handeling a button (With SENSE)
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