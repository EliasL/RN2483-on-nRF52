#include "nRF52_board_config.h"
#include "inc/nRF52_uart.h"
#include "inc/rn2483.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"

// General variables
char buffer[RN2483_MAX_BUFF];
int ret = -1;


int shut_down(){
    
    for(int i = 0; i < 4; i++)
    {
        led_toggle(LED_GPIO);
        wait_a_bit(0.1);
    }
    return 1;
}

int ready(){
    for(int i = 0; i < 10; i++)
    {
        led_toggle(LED_GPIO);
        wait_a_bit(0.05);
    }
    led_off(LED_GPIO);
    return 1;
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
        shut_down();
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

int main(void)
{
//Init

    //init nrf52
    debug_print("-\n       Initializing TDX on pin %d and RDX on pin %d\n-", nRF52_PIN_TXD, nRF52_PIN_RXD);
    nRF52_uart_init();

    //init led and button
    //led_init(LED_GPIO);
    button_init(BUTTON_GPIO);

    //Autobaud
    if(!autobaud()){
        // The autobauding attempt has failed, and the device is/has been reset
        return shut_down();
    }
    led_toggle(LED_GPIO);

    //init rn2483
    if(!init_rn2483()){
        // The init of the RN2483 has failed.
        return shut_down();
    }
    led_toggle(LED_GPIO);

    //Join
    if(!join()){
        // The RN2483 was unable to connect/join the network
        return shut_down();
    }
    led_toggle(LED_GPIO);

    //Send message
    if(!send("test")){
        // The message was not sent
        return shut_down();
    }
    
    ready();
    //Wait
    while (1)
    {
        if(button_is_pressed(BUTTON_GPIO)){
            led_on(LED_GPIO);
            send("God jul!");
            ready();
        }
    }
    nRF52_uart_quit();
    return 0;
}