
#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"
#include "application.h"

int main(){
    
    application_init();

    while(true){
    application_awake_cycle();

    //Let things calm down a bit
    wait_a_bit(0.1);
    debug_print("Sleeping...");

    // Enter System ON sleep mode
    __WFE();
    __SEV();
    __WFE();
    
    debug_print("AWAKE!");
    }
    return 1;
}