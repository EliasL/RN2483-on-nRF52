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