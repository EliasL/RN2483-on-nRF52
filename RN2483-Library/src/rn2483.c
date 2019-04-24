/*!
	@file	rn2483.c
	@authors	Alexander Collins (alexander-collins@outlook.cm)    Elias Lundheim (elias@tradlosetrondheim.no)
	@date	September 2017 - April 2018                                 August 2018 - January 2019
	@brief	A library for nRF52 that allows for interaction with a Microchip RN2483.
	@see rn2483.h
*/
//========
// MACROS
//========
//includes
#include "rn2483.h"
#include "nRF52_uart.h"
#include "nRF52_board_config.h"
#include "Simple_debug_printf.h"
#include "utilities_lib.h"
//===========
// FUNCTIONS
//===========
//PRIVATE
// Converts buff into a string representation of it hexadecimal representation
static void get_hex_string(const uint8_t *buff, char *ret)
{
    int i, len;
    len = length_of_string(buff);

    for(i = 0; i<len; i++){
        sprintf(ret+i*2, "%02X", buff[i]);
    }
    return;
}
// Converts string a string representation of hex characters to ascii
static void get_text_string(const char *hex, int hex_len, char *ret)
{
    char byte[3];

    for (int i = 0; i < hex_len; i+=2)
    {
        byte[0] = hex[i];
        byte[1] = hex[i+1];
        byte[2] = '\0';

        ret[i/2] = (char)strtol(byte, NULL, 16);
    }

    return;
}

// Reads from the RX buffer into response until '\n' or EOB
static int RN2483_response(uint8_t *buffer)
{
    int commandLength;
    commandLength = nRF52_UART_blocking_read(buffer);

    if (strcmp((char*)buffer, "ok\r\n")){
        return RN2483_SUCCESS;
    }
    else if (strcmp((char*)buffer, "invalid param\r\n")){
        return RN2483_ERR_PARAM;
    }
    else if (strcmp((char*)buffer, "err\r\n")){
        return RN2483_ERR_PANIC;
    }
    else if (commandLength == 0 ){
        return RN2483_ERR_PANIC;
    }
    else if (commandLength == 35){
        // This section handles "RN2483 1.0.1 Dec 15 2015 09:38:09" like responces
        return RN2483_SUCCESS;
    }
    else{
        debug_print("Unhandled responce from RN2483!");
        debug_print("Add appropriate responce for %s", buffer);
        return RN2483_ERR_PANIC;
    }
}

//PUBLIC
//system
// Resets the RN2483 by toggling the RESET pin
int RN2483_reset()
{

    uint8_t *ver = (uint8_t *)malloc(RN2483_MAX_BUFF);
    int ret = RN2483_response(ver);   //firmware version should be in buff (RN2483...)

    if (ret != RN2483_ERR_PANIC)
        ret = RN2483_SUCCESS;

    free(ver);
    return ret;
}

// Attempts to trigger the auto-baud detection sequence.
int RN2483_autobaud()
{
    
    char buff[RN2483_MAX_BUFF] = "";

    debug_print("Sending autobaud sequence");
    nRF52_UART_autobaud();
    wait_a_bit(0.01);
    RN2483_firmware(buff);

    // Check if the beginning of the responce is R (From "RN2483 1.0.1 Dec 15 2015 09:38:09" like responces)
    if(buff[0] == 'R'){
        debug_print("Firmware: %s", buff);
        debug_print("Done with autobauding");
        return RN2483_SUCCESS;
    }
    else
    {
        return RN2483_ERR_PANIC; // Failure
    }
}

// Sends a command to the RN2483 and sets the response in buffer
int RN2483_command(const char *command, char *response)
{
	//check command ends with \r\n (easy to forget)
	int end = strlen(command);
	if (command[end-2] != '\r' || command[end-1] != '\n')
		return RN2483_ERR_PARAM;

    int attempts = 0;
    int ret;
    response[0] = ' ';
    while(attempts <= 5 && (response[0] != 'o' || response[0] != 'R')){ // We want either ok or RN2483
        //send command
        nRF52_UART_write((uint8_t *)command);
        debug_print("Sending command: %s", command);

        //recv response
        ret = RN2483_response((uint8_t *)response);
        debug_print("Response: %s", response);


        if (ret != RN2483_ERR_PANIC)
            return RN2483_SUCCESS;
        attempts++;
    }
    return ret;
}

// Retrieves the firmware version of the RN2483 module and stores it in buff.
int RN2483_firmware( char *buff)
{
	return RN2483_command("sys get ver\r\n", buff);
}


//LoRa
// Initialises all the RN2483 MAC settings required to run LoRa commands (join, tx, etc).
int RN2483_initMAC()
{
    RN2483_autobaud();
    int i = -1;
	int ret = RN2483_ERR_PANIC;
	char *response = (char *)malloc(RN2483_MAX_BUFF);

	do {
		i++;
        memset(response, '\0', RN2483_MAX_BUFF);

		switch(i)
		{
            case 0:	//reset MAC
                #ifdef LoRaWAN_Frequency
                    ret = RN2483_command("mac reset " LoRaWAN_Frequency "\r\n", response);
                #endif
                break;
            case 1:	//set DevAddr
                #ifdef LoRaWAN_DevAddr
                    ret = RN2483_command("mac set devaddr " LoRaWAN_DevAddr "\r\n", response);
                #endif
                break;
            case 2:	//set DevEui
                #ifdef LoRaWAN_DevEUI
                    ret = RN2483_command("mac set deveui " LoRaWAN_DevEUI "\r\n", response);
                #endif
                break;
            case 3:	//set AppEui
                #ifdef LoRaWAN_AppEUI
                    ret = RN2483_command("mac set appeui " LoRaWAN_AppEUI "\r\n", response);
                #endif
                break;
            case 4:	//set AppKey
                #ifdef LoRaWAN_AppKey
                    ret = RN2483_command("mac set appkey " LoRaWAN_AppKey "\r\n", response);
                #endif
                break;
            case 5:	//set DataRate
                #ifdef LoRaWAN_DataRate
                    ret = RN2483_command("mac set dr " LoRaWAN_DataRate "\r\n", response);
                #endif
                break;
             case 6: //set NwkSKey
                #ifdef LoRaWAN_NwkSKey
                    ret = RN2483_command("mac set nwkskey " LoRaWAN_NwkSKey "\r\n", response);
                #endif
                break;
            case 7: //set AppSkey
                #ifdef LoRaWAN_AppSKey
                    ret = RN2483_command("mac set appskey " LoRaWAN_AppSKey "\r\n", response);
                #endif
                break;
            case 8: //set ADR
                #ifdef LoRaWAN_ADR
                    ret = RN2483_command("mac set adr " LoRaWAN_ADR "\r\n", response);
                #endif
                break;
            case 9: //save
                ret = RN2483_command("mac save\r\n", response);
                break;

		}
    } while (i < 10 && ret == RN2483_SUCCESS);
	return ret;
}

// Attempts to join a LoRa network using the specified mode.
int RN2483_join(int mode)
{
	int ret = RN2483_ERR_PANIC;
	char *response = (char *)malloc(RN2483_MAX_BUFF);

    // send command & recv initial response
	if (mode == RN2483_OTAA)
		ret = RN2483_command("mac join otaa\r\n", response);
	else if (mode == RN2483_ABP)
		ret = RN2483_command("mac join abp\r\n", response);
	else
		ret = RN2483_ERR_PARAM;
    
    if (ret == RN2483_SUCCESS)
    {
        // if initial response success, wait for network response
        if (strcmp(response, "ok\r\n") == 0)
        {
            debug_print("Waiting for network responce...");
            RN2483_response((uint8_t *)response);
            debug_print("Network responce: %s", response);
            if (strcmp(response, "accepted\r\n") == 0)
                ret = RN2483_SUCCESS;
            else if (strcmp(response, "denied\r\n") == 0)
                ret = RN2483_DENIED;
            else
                ret = RN2483_ERR_PANIC;
        }
        // else return err code
        else if (strcmp(response, "keys_not_init\r\n") == 0)
            ret = RN2483_ERR_KIDS;
        else if (strcmp(response, "no_free_ch\r\n") == 0)
            ret = RN2483_ERR_BUSY;
        else if (strcmp(response, "silent\r\n") == 0 || strcmp(response, "busy\r\n") == 0 || strcmp(response, "mac_paused\r\n") == 0)
            ret = RN2483_ERR_STATE;
        else
            ret = RN2483_ERR_PANIC;
	}
    return ret;
}

// Sends a confirmed/unconfirmed frame with an application payload of buff.
int RN2483_tx(const char *buff, bool confirm, char *downlink)
{
    int ret = RN2483_ERR_PANIC;
    char *response = (char *)malloc(RN2483_MAX_BUFF);
    memset(response, '\0', RN2483_MAX_BUFF);

    // figure out max payload length based on data rate
    int max_len = 0;
    #ifdef LoRaWAN_DataRate
	if (strcmp(LoRaWAN_DataRate, "0") == 0 || strcmp(LoRaWAN_DataRate, "1") == 0 || strcmp(LoRaWAN_DataRate, "2") == 0)
		max_len = 59;
	else if (strcmp(LoRaWAN_DataRate, "3") == 0)
		max_len = 123;
	else if (strcmp(LoRaWAN_DataRate, "4") == 0 || strcmp(LoRaWAN_DataRate, "5") == 0 || strcmp(LoRaWAN_DataRate, "6") == 0 || strcmp(LoRaWAN_DataRate, "7") == 0)
		max_len = 230;
	else
		max_len = 230;
    #endif

    // get payload
    char *payload = (char *)malloc(strlen(buff)*2 + 1); // 1 byte = 2 hex chars
    get_hex_string((const uint8_t *)buff, payload);

    // send command
    char *cmd = (char *)malloc(max_len);
    if (confirm)
        sprintf(cmd, "mac tx cnf %s %s\r\n", LoRaWAN_Port, payload);
    else
        sprintf(cmd, "mac tx uncnf %s %s\r\n", LoRaWAN_Port, payload);
    ret = RN2483_command(cmd, response);
    free(cmd);

    if (ret == RN2483_SUCCESS)
    {
        // if initial response success, wait for tx success
        if (strcmp(response, "ok\r\n") == 0)
        {
            memset(response, '\0', RN2483_MAX_BUFF);
            RN2483_response((uint8_t *)response);
            
            if (strcmp(response, "mac_tx_ok\r\n") == 0)
                ret = RN2483_NODOWN;
            else if (strcmp(response, "mac_err\r\n") == 0 || strcmp(response, "invalid_data_len\r\n") == 0)
                ret = RN2483_ERR_PANIC;
            else //assume downlink data (mac_rx <port> <data>)
            {
                //convert 'hex' to 'ascii' equivalent
                char *hex_data = (char *)malloc(strlen(response-8));
                memcpy(hex_data, &response[9], strlen(response)-9);
                get_text_string(hex_data, strlen(hex_data), downlink);
                free(hex_data);
                ret = RN2483_SUCCESS;
            }
        }
        // else return err code
        else if (strcmp(response, "invalid_param\r\n") == 0)
            ret = RN2483_ERR_PARAM;
        else if (strcmp(response, "no_free_ch\r\n") == 0){
            debug_print("No channels are available, please wait and try again.");
            ret = RN2483_ERR_BUSY;
        }
        else if (strcmp(response, "not_joined\r\n") == 0 || strcmp(response, "frame_counter_err_rejoin_needed\r\n") == 0)
            ret = RN2483_ERR_JOIN;
        else if (strcmp(response, "silent\r\n") == 0 || strcmp(response, "busy\r\n") == 0 || strcmp(response, "mac_paused\r\n") == 0)
            ret = RN2483_ERR_STATE;
        else
            ret = RN2483_ERR_PANIC;
    }
    
    free(response);
    return ret;
}

void RN2483_sleep(const unsigned int ms)
{   
    debug_print("Putting RN to sleep");
    int max_len = 59;
    char *cmd = (char *)malloc(max_len);
    sprintf(cmd, "sys sleep %u\r\n", ms);
    nRF52_UART_write((const uint8_t*)cmd);
}
