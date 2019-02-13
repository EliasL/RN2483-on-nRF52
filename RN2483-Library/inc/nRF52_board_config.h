

/*
Your nRF52 board should be connected to the RN2483 in some way. 
Depending on how you have connected the two devices,
you need to change these values accordingly.
(Or you could leave them as they are, and change your connections instead)

*/
#define BUTTON_GPIO  (16UL)
#define LED_GPIO  (17UL)
#define nRF52_PIN_TXD (22) 
#define nRF52_PIN_RXD (23)
#define nRF52_RESET_PIN (27) // Sometimes not soldered on some shields

//Non-board related definitions
#define PIN_HIGH (1)
#define PIN_LOW (0)