
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
void application_sleep();


/*
* Function: autobaud
* ----------------------------
*   Tries to autobaud 5 times
*
*   return: autobaud was successful
*/
bool autobaud();


/*
* Function: init_RN2483
* ----------------------------
*   inits the RN2483
*
*   return: init was successful
*/
bool init_RN2483();

/*
* Function: join
* ----------------------------
*   The RN2483 attempts to join the LoraWAN network
*
*   return: join was successful
*/
bool join();

/*
* Function: send
* ----------------------------
*   Tries to send a message. If there is a network error, it returns false,
*   but for some errors, it redoes the RN2483 init
*
*   message: String of data to be sent
*   retryDelay: 
*
*   return: send was successful
*/
bool send(const char* message);


/*
* Function: application_init
* ----------------------------
*   Initiates application
*
*   retryDelay: Delay bettween retry if something goes wrong
*   nRF_init: do nRF related inits? (Always performs RN init) 
*/
void application_init();

/*
* Function: application_awake_cycle
* ----------------------------
*   Sends temperature
*
*   sleepDuration: Duration of sleep after successfully sending data
*   retryDelay: Delay bettween retry if something goes wrong
*/
void application_awake_cycle();