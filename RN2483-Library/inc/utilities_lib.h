#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
*  Runs a long loop
*/
void wait_a_bit(float bits);