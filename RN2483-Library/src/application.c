#include "application.h"

#define SLEEPDURATION 20000

// General variables
char buffer[RN2483_MAX_BUFF];
int ret = -1;

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

void application_sleep(){
    debug_print("Entering sleep");
    RN2483_sleep(SLEEPDURATION);
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

bool send(const char* message){
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
            application_sleep();
        }
        return false;
    }
    else if(ret == RN2483_ERR_PANIC){
        debug_print("Something went very wrong...");
        // We have to redo the init
        debug_print("Re-initializing...");
        application_init();
        return false;
    }
    else{
        debug_print("Something went wrong...");
        // Maybe busy or something, but no need to redo init
        return false;
    }
}

void application_init(){
    //Init start ----

    //init nrf52
    debug_print("-\n       Initializing TDX on pin %d and RDX on pin %d\n-", nRF52_PIN_TXD, nRF52_PIN_RXD);
    nRF52_UART_init();
    //init_nRF52_Timer_RTC0(); // init timer

    //init led and button
    #if LIGHT
        led_init(LED_GPIO);
    #endif
    button_init(BUTTON_GPIO);
}

void application_awake_cycle()
{    
    //init rn2483
    while(!init_RN2483()){wait_a_bit(3);}
    led_toggle(LED_GPIO);
    //Init end

    //join
    while(!join()){wait_a_bit(100);}
    led_toggle(LED_GPIO);

    //measure temperature
    int temperature = (int)get_temperature();
    //send temperature
    char temp[5];
    snprintf(temp, 5, "%d", temperature);
    debug_print("Read temperature: %s", temp);
    send(temp);
    //Sleep
    application_sleep();
}