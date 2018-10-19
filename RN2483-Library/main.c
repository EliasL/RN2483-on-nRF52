#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"

// General variables
char buffer[RN2483_MAX_BUFF];
int ret = -1;


int main(void)
{
//Init

    //init nrf52
    debug_print("-\n       Initializing TDX on pin %d and RDX on pin %d\n-", nRF52_PIN_TXD, nRF52_PIN_RXD);
    nRF52_uart_init();

    //Autobaud
    int tries = 0;
    int maxAttempts = 10;
    debug_print("Autobauding...");
    do {
        tries++;
        if(tries>1){
            debug_print("Retrying autobauding. Attempt %d/%d.", tries, maxAttempts);
            wait_a_bit(0.1);
        }
        ret = RN2483_autobaud();
    } while (ret != RN2483_SUCCESS && tries < maxAttempts);
    if (ret == RN2483_SUCCESS){
    }
    else
    {
        debug_print("Failed to autobaud! Resetting nRF52...");
        NVIC_SystemReset();
        return 1;
    }

    //init rn2483
    debug_print("Init RN2483 MAC..");
    if (RN2483_initMAC() != RN2483_SUCCESS)
    {
        debug_print("RN2483 initMAC error!");
        return 1;
    }
    debug_print("Init RN2483 MAC success!");

//Join
    
    tries = 0;
    maxAttempts = 4;
    debug_print("\nAttempting to connect...");
    do {
        tries++;
        if(tries>1){
            debug_print("Retrying connection. Attempt %d/%d.", tries, maxAttempts);
        }
        ret = RN2483_join(RN2483_OTAA);
    } while (ret != RN2483_SUCCESS && tries < maxAttempts);

    if (ret == RN2483_SUCCESS){
    }
    else
    {
        debug_print("Failed to join...");
        return 1;
    }

    debug_print("Sending data...");

//Transmitt (if joined)
    if (ret == RN2483_SUCCESS)
    {
        ret = RN2483_tx("abcdefgijklmnopqrstuvwxyz", true, buffer);
        if (ret == RN2483_SUCCESS || ret == RN2483_NODOWN)
        {
            debug_print("Data sent!");
        }
        else{
            debug_print("Something went wrong...");
        }
    }
    nRF52_uart_quit();

//Wait
    while (1)
    {
        __WFE();
    }
    return 0;
}