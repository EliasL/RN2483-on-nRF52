
#include "nRF52_board_config.h"
#include "nRF52_uart.h"
#include "rn2483.h"
#include "../RTT/Simple_debug_printf.h"
#include "utilities_lib.h"


/*

Application awake cycle

The application refers to both the nRF52 and the RN2483 together as a single component.
The application_awake_cycle function will start by initializing the nRF's UART and 
connecting to the RN, then do something, and finally end by putting both of the components
back into sleep.

main.c dictates exactly how and when this cycle will be started.

Notes:
    If the nRF has successfully autobauded, we will assume that any sleep command
    sent to the RN is successfull if it does not respond. We can then rely on the
    RN waking up the nRF. However, if for some reason the autobaud fails, can not
    go to sleep as we usually would, simply waiting for the RN to wake the nRF.
    For example, say the RN is completely unresponsive due to temperatures below -40,
    but the nRF is still working. There is no reason to constantly try to autobaud,
    and restart. We want to conserve energy for when the temperature rises.
    This means we need one sleep function for an operational RN, and one sleep
    function working independant of the RN.

*/


//defines
#define LIGHT true


/*
* Function: application_sleep
* ----------------------------
*   Sends the RN a sleep command with the given sleepDuration as argument.
*   Then powers down the nRF.
*
*   sleepDuration: time in ms, must be between 100 and 4294967295
*/
void application_sleep(const unsigned int sleepDuration);


/*
* Function: autobaud
* ----------------------------
*   Tried to autobaud 5 times, and resets the nRF if the autobaud is unsuccessful. 
*   

*/
bool autobaud(){

    //Autobaud
    int tries = 0;
    int maxAttempts = 5;
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
        return true;
    }
    else
    {
        debug_print("Failed to autobaud! Resetting nRF52...");
        wait_a_bit(0.5);
        NVIC_SystemReset(); // Any code beyond this line is not run, because the device has restarted
        wait_a_bit(0.1);
        return false;
    }
}

bool init_rn2483(){
    //init rn2483
    debug_print("Init RN2483 MAC..");
    if (RN2483_initMAC() != RN2483_SUCCESS)
    {
        debug_print("RN2483 initMAC error!");
        return false;
    }
    else{
        debug_print("Init RN2483 MAC success!");
        return true;
    }
}

bool join(){
    //Join
    int tries = 0;
    int maxAttempts = 4;
    debug_print("\nAttempting to connect...");
    do {
        tries++;
        if(tries>1){
            debug_print("Retrying connection. Attempt %d/%d.", tries, maxAttempts);
        }
        ret = RN2483_join(RN2483_OTAA);
    } while (ret != RN2483_SUCCESS && tries < maxAttempts);

    if (ret == RN2483_SUCCESS){
        return true;
    }
    else
    {
        debug_print("Failed to join...");
        return false;
    }
}

bool send(const char* message){
    debug_print("Sending data...");

    //Transmitt
    ret = RN2483_tx(message, true, buffer);
    if (ret == RN2483_SUCCESS || ret == RN2483_NODOWN)
    {
        debug_print("Data sent!");
        return true;
    }
    else{
        debug_print("Something went wrong...");
        return false;
    }
}

void application_awake_cycle(const unsigned int sleepDuration, const unsigned int retryDelay)
{
    //Init start ----
    //init nrf52
    debug_print("-\n       Initializing TDX on pin %d and RDX on pin %d\n-", nRF52_PIN_TXD, nRF52_PIN_RXD);
    nRF52_uart_init();

    //init led and button
    #if LIGHT
        led_init(LED_GPIO);
    #endif
    button_init(BUTTON_GPIO);

    //autobaud
    if(!autobaud()){
        // The autobauding attempt has failed, and the device is/has been reset
    }
    led_toggle(LED_GPIO);

    //init rn2483
    if(!init_rn2483()){
        // The init of the RN2483 has failed.
        application_sleep(retryDelay);
    }
    led_toggle(LED_GPIO);
    //Init end

    //Send start ----
    //join
    if(!join()){
        // The RN2483 was unable to connect/join the network
        application_sleep(retryDelay);
    }
    led_toggle(LED_GPIO);

    //send message
    if(!send("test")){
        // The message was not sent
        application_sleep(retryDelay);
    }
    //Send end

    //Sleep start ----
    //put both nRF and RN to sleep
    application_sleep(sleepDuration);
    //Sleep end
}