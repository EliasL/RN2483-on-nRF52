
#include "application_awake_cycle.h"

// General variables
char buffer[RN2483_MAX_BUFF];
int ret = -1;

void application_sleep(const unsigned int sleepDuration){
    
    RN2483_sleep(sleepDuration);
    NRF_POWER->SYSTEMOFF = 1;
}

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