#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"


/*
Low power radio

We want both the nRF(nRF52832) and the RN(RN2483) to be asleep as often as possible.
The RN does not support unlimited sleep (sleep until woken), therefore the nRF must 
occasionally come be awake in order to put the RN to sleep. 

When the RN wakes up, it sends "ok\r\n" over UART. We want to use this in order to wake
the nRF. Since we do not need to know what the RN sent (but just assume it will always 
be "ok\r\n"), we can let the nRF enter a very deep sleep.

We use the SENSE option on the RX pin in order to achieve this. We will also enable SENSE 
on one of the buttons, so that the nRF can be woken up externally. 

*/
