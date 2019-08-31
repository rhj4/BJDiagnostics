#pragma once
// Config.h
#define TRACING				true

//	Pick one of the following:
#define BOARD_TYPE_MEGA		false
#define BOARD_TYPE_NANO		true
#define BOARD_TYPE_UNO		false

//	Pick one of the following:
#define PROTOCOL_WIFI		true
#define PROTOCOL_BLUETOOTH	false
#define PROTOCOL_WIRED		false

//	Pick one of the following:
#define ROLE_IS_GETTER		false
#define ROLE_IS_PUTTER		true 


//	Define pins used by WiFi hardware
//	Light LED on this pin if radio active
#if PROTOCOL_WIFI
#define CONNECTED_ACTIVE_LED	4
#elif PROTOCOL_BLUETOOTH
#define CONNECTED_ACTIVE_LED	4
#elif PROTOCOL_WIRED
#define CONNECTED_ACTIVE_LED	15					
#endif
#define CONNECTED_ACTIVE_COLOR	"GREEN"

//	Light LED on this pin when radio is listening
#if PROTOCOL_WIFI
#define LISTENING_LED			5
#elif PROTOCOL_BLUETOOTH
#define LISTENING_LED			5
#elif PROTOCOL_WIRED
#define LISTENING_LED			19
#endif
#define LISTENING_COLOR			"RED"

//	Light LED on this pin when radio is sending
#if PROTOCOL_WIFI
#define SENDING_LED				6
#elif PROTOCOL_BLUETOOTH
#define SENDING_LED				6
#elif PROTOCOL_WIRED
#define SENDING_LED				17
#endif
#define SENDING_COLOR			"BLUE"

//	Flash LED on this pin if role is Transmitter
#if PROTOCOL_WIFI
#define TRANSMITTER_LED			7
#elif PROTOCOL_BLUETOOTH
#define TRANSMITTER_LED			7
#elif PROTOCOL_WIRED
#define TRANSMITTER_LED			14
#endif
#define TRANSMITTER_COLOR		"YELLOW"

#if PROTOCOL_WIFI
//	5 byte name of PUTTER device
#define WIFI_REMOTE_ADDRESS		{ 'R','E','M','O','T' }

//	5 byte name of GETTER
#define WIFI_ROBOT_ADDRESS		{ 'R','O','B','O','T' }

//	Power Amplifier level - increase for more distance
#define WIFI_POWER_LEVEL		RF24_PA_LOW		

//	We don't need lots of speed
#define WIFI_DATA_RATE			RF24_2MBPS			

//	Unlikely to be used by WiFi, Microwave ovens, etc
#define WIFI_DATA_CHANNEL		124					

#define WIFI_CRC_RATE			RF24_CRC_DISABLED

//	Wait for up to 1000 millis for receive to complete
#define WIFI_RECEIVE_TIMEOUT	1000			

//	# of retries on confirm message
#define	WIFI_RETRY_COUNT		5

//	Send ACK string upon receipt of each packet
#define WIFI_SEND_ACK			false

//	Define the pins used by the nRF24L01 module
#if BOARD_TYPE_MEGA
#define WIFI_CE_PIN				48
#define WIFI_CSN_PIN			49
#define WIFI_MISO_PIN			50
#define WIFI_MOSI_PIN			51
#define WIFI_SCK_PIN			52

#else // Board type is Nano or Uno
#define WIFI_CE_PIN				14
#define WIFI_CSN_PIN			15
#define WIFI_MISO_PIN			11
#define WIFI_MOSI_PIN			12
#define WIFI_SCK_PIN			13
#endif /* BOARD_TYPE */
#define NAME					(char*) "Serial Wired Transmitter Tester"

#elif PROTOCOL_BLUETOOTH
#define HC05_BAUD_RATE			9600	//	HC05 default BAUD rate
#define HC05_LED_FLASH_COUNT	3		//	# of times to flash led
#define HC05_LED_FLASH_LENGTH	500		//	MS duration of each flash

#if BOARD_TYPE_MEGA
#define HC05_TX_PIN				48		//	Transmit pin
#define HC05_RX_PIN				49		//	Receive pin
#define HC05_STATE_PIN			50		//	HC05 State 
#define HC05_STATE_LED			51		//	LED to turn on if paired
#else // Board is Nano or Uno
#define HC05_TX_PIN				11		//	Transmit pin
#define HC05_RX_PIN				12		//	Receive pin
#define HC05_STATE_PIN			11		//	HC05 State 
#define HC05_STATE_LED			12		//	LED to turn on if paired
#endif /* BOARD_TYPE */
#define NAME					(char*) "Bluetooth Transmitter Tester"

#elif PROTOCOL_WIRED
#define TX_PIN					7
#define RX_PIN					8

#define SENDER_LED				A3
#define RECEIVER_LED			A0

#define NAME					(char*) "Serial Wired Transmitter Tester"
#endif /* PROTOCOL_ */

#define MESSAGE_SIZE 100

#define COLS 10
#define LED_PIN 4
#define DATA_BUFFER_SIZE 60	//	Serial buffer size is 64
#define LOOP_DELAY 100
#define FLASH_COUNT 10
#define FLASH_DURATION 50

