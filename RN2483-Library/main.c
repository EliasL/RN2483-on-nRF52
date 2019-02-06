#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"
#include "application.h"

/*
Low power radio
*/

int main(){
    const unsigned int retryDelay = 1;
    const unsigned int sleepDelay = 1;
    
    application_init(retryDelay, true);
    while(true){
        application_awake_cycle(retryDelay, sleepDelay);
    }
    return 1;
}