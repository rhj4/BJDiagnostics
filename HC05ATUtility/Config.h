#pragma once

/*******************************************************************
*
*	BUTTON MODES
*
********************************************************************/

//	Return the button state as soon as it changes
//	without verifying the current state
#define BUTTON_MODE_IMMEDIATE		0

//	Return current button state, but only when
//	the state changes
#define BUTTON_MODE_RAW				1

//	Return true when pressed, false when released
#define BUTTON_MODE_SIMPLE			2

//	Capture when switch pressed, wait until released
//	On release, toggle switch value and return it.
#define BUTTON_MODE_TOGGLE			3

//	Cycle switch value through (0..<states>) and
//	return current state when released
#define BUTTON_MODE_MULTI_STATE		4

#define MAX_BUTTON_STATE			10

/*******************************************************************
*
*	PIN ASSIGNMENTS
*
********************************************************************/
#define SLC					19
#define SDA					18
#define RX					15		//	GREEN
#define TX					14		//	ORANGE
#define POWER_LED_PIN		12		//	RED
#define ATMODE_LED_PIN		11		//	GREEN
#define ENABLED_LED_PIN		10		//	BlUE
#define STATE_LED_PIN		9		//	YELLOW
#define MODE_LED_PIN		16		//	WHITE
#define STATUS_LED_PIN		17		//  WHITE
#define ENA					6		//	YELLOW - 
#define POWER_PIN			3		//	BROWN - Goes high when power is cut
#define STATE_PIN			2		//	WHITE

#define BUTTON_1_PIN		5		//	POWER Toggle 
#define BUTTON_1_LED		17		//	WHITE LED
#define BUTTON_1_STATES		2
#define BUTTON_1_NAME		"POWER:"
#define BUTTON_1_MODE		BUTTON_MODE_TOGGLE

#define BUTTON_2_PIN		8		//	MODE Toggle
#define BUTTON_2_LED		16		//	WHITE LED
#define BUTTON_2_STATES		2
#define BUTTON_2_NAME		"MODE:"
#define BUTTON_2_MODE		BUTTON_MODE_SIMPLE

//	Internal Variables
#define BT_WAIT		50		// Settling time
#define LOOP_WAIT	100		//	Loop settling time

//	Error codes
#define NO_ERROR_FOUND              0
#define MODULE_NOT_AVAILABLE		101
#define RESPONSE_BUFFER_EMPTY		102
#define NO_DATA_RETURNED			103
#define BUFFER_SIZE					50

#define DEBOUNCE_DELAY 50
#define FLASH_TIMES		5
#define FLASH_DURATION	50

#define TRACING			true

//	Uncomment the following line to test
//	each switch mechanically
#define TEST_SWITCHES	false
#define TEST_MILLIS		10000

#define COLS			20