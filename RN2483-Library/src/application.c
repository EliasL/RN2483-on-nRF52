#include "application.h"

// General variables
char buffer[RN2483_MAX_BUFF];
int ret = -1;

void application_sleep(const unsigned int sleepDuration){
    
    ret = RN2483_sleep(sleepDuration);
    if(ret==RN2483_SUCCESS){
        sleep();
    }

    else{
        self_monitored_sleep(1); // Use 1 for testing, and use something like 64 or 256 otherwise.  
    }
}

void self_monitored_sleep(const unsigned int sleepDurationMinutes){
    wake_up_after_minutes(sleepDurationMinutes);
}

bool autobaud(void){

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
        debug_print("Failed to autobaud!");
        return false;
    }
}

bool init_RN2483(void){
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

bool join(void){
    //Join
    int tries = 0;
    int maxAttempts = 4;
    debug_print("\nAttempting to join network...");
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

bool send(const char* message, const unsigned int retryDelay){
    debug_print("Sending data...");

    //Transmitt
    ret = RN2483_tx(message, true, buffer);
    if (ret == RN2483_SUCCESS || ret == RN2483_NODOWN)
    {
        debug_print("Data sent!");
        return true;
    }
    else if(ret == RN2483_ERR_JOIN){
        debug_print("Couldn't join...");
        // We have to redo the init
        debug_print("Rejoining...");
        while(!join()){
            // The RN2483 was unable to connect/join the network
            application_sleep(retryDelay);
        }
        return false;
    }
    else if(ret == RN2483_ERR_PANIC){
        debug_print("Something went very wrong...");
        // We have to redo the init
        debug_print("Re-initializing...");
        application_init(retryDelay, false);
        return false;
    }
    else{
        debug_print("Something went wrong...");
        // Maybe busy or something, but no need to redo init
        return false;
    }
}

void application_init(const unsigned int retryDelay, bool nRF_init){
    //Init start ----
    if(nRF_init){
        //init nrf52
        debug_print("-\n       Initializing TDX on pin %d and RDX on pin %d\n-", nRF52_PIN_TXD, nRF52_PIN_RXD);
        nRF52_uart_init();
        init_nRF52_Timer_RTC0(); // init timer

        //init led and button
        #if LIGHT
            led_init(LED_GPIO);
        #endif
        button_init(BUTTON_GPIO);
    }

    //autobaud
    while(!autobaud()){
        // The autobauding attempt has failed
        // DO NOT use application_sleep, since the autobaud has failed, the RN might be unresponsive. This will make
        // the nRF think the RN has succesfully gone to sleep. If application_sleep is used here, the nRF might sleep forever
        self_monitored_sleep(1); // Use 1 for testing, otherwise maybe 64 or 256
    }
    led_toggle(LED_GPIO);

    //init rn2483
    while(!init_RN2483()){
        // The init of the RN2483 has failed.
        application_sleep(retryDelay);
    }
    led_toggle(LED_GPIO);
    //Init end
    
    //join
    while(!join()){
        // The RN2483 was unable to connect/join the network
        application_sleep(retryDelay);
    }
    led_toggle(LED_GPIO);

}

void application_awake_cycle(const unsigned int sleepDuration, const unsigned int retryDelay)
{
    //measure temperature

    //send temperature
    while(!send("test", retryDelay)){
        // The message was not sent
        application_sleep(retryDelay);
    }

    //Sleep
    application_sleep(sleepDuration);
}