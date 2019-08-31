#ifndef _BJHC05Utility_h
#define _BJHC05Utility_h
/********************************************************************
*
*	Name:		BJHC05Utility.h
*	Created:	5/20/2018 3:48:10 PM
*	Author:		Bob Jones <bob@rhj4.com>
*
*	Desc:		Manage interactions with HC05 module
*
*	LEDs:		There are four LEDs that indicate the status of
*				the connection with the HC05 module:
*
*	Power:		If the VCC pin on the module is connected with
*				3.3V (RED), then the POWER_LED (RED:12) will be on.
*				Putting the HC05 into AT mode requires that:
*				1) Power be cut to the module
*				2) The button on the module must be pressed AS
*				POWER IS RECONNECTED. This is accomplished on
*				the board by unplugging the red jumper that runs
*				from 3.3V to Row D, Col 29 on the breadboard.
*				The red jumper that runs from Row A, Col 28-25
*				acts as a switch to disconnect the power.
*
*	Enabled:	The EN pin on the HC05 must be set to high when
*				the module is powered up in order to enter AT mode.
*				For the module to return to normal mode, this pin
*				must be set to LOW. The jumper running from D6 to
*				row I, col 30 on the breadboard is the "switch" for
*				the EN pin. If this pin is HIGH, the BLUE LED
*				connected to D10 will be on.
*
*	AT Mode:	If the board has entered AT mode, the GREEN LED
*				connected to D11 will be on.
*
*/

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SSD1306init.h>
#include <SSD1306AsciiWire.h>
#include <SSD1306AsciiAvrI2c.h>
#include <SSD1306Ascii.h>
#include "Config.h"
#include "BJDriverButton.h"

#define MODE_AUTOMATIC	false	//	Automatically try to connect 
#define MODE_SIMPLE		true	//	Manual connection required
#define MODE_NONE		false	//	Just send what is entered in serial port

//#define OLED_RESET		4
#define ROW_HEIGHT		8
#define MIN_ROW			2
#define MAX_ROW			8

 /***************************************************************
 *
 *	CLASS DEFINITION
 *
 ****************************************************************/
class BJHC05Utility {

	//	PRIVATE PROPERTIES & METHODS
private:
	bool _tracing = TRACING;

	/*******************************************************************
	*
	*	I2C OLED Variables
	*
	********************************************************************/
	// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

	SSD1306AsciiWire oled;

	uint8_t col0 = 0;  // First value column
	uint8_t col1 = 0;  // Last value column.
	uint8_t rows;      // Rows per line.

	/*******************************************************************
	*
	*	BUTTON DEFINITIONS
	*
	********************************************************************/

	char *buttonSwitch1 = ( char* ) BUTTON_1_NAME;
	BJDriverButton *powerButton;

	//	Switch 2 is used to cut the power to the HC05
	char *buttonSwitch2 = ( char* ) BUTTON_2_NAME;
	BJDriverButton *modeButton;

	const char* label [ 2 ] {
		BUTTON_1_NAME,
		BUTTON_2_NAME,
	};

	/***************************************************************
	*
	*	CLASS PROPERTIES
	*
	****************************************************************/

	int _lastErrorNumber = -1;

	//	True when module is in AT Command Mode
	bool _ATCommandModeEnabled = false;

	bool _powerIsOn = true;
	bool _powerCut = false;

	bool _normalMode = false;

	//	True when switch is connected
	bool _powerConnected = false;

	bool _stateEnabled = false;

	bool _ATModeEnabled = false;
	bool _enabled = false;
	bool _moduleRead = false;

	int _currentState = 0;
	int _lastState = 0;

	char _response [ 20 ] = {};
	int _responseIndex = 0;

	/***************************************************************
	*
	*	AT COMMAND STRINGS
	*
	****************************************************************/
	//	Useful strings:
	char *AT = ( char* )"AT\n\r";
	char *OK = ( char* )"OK\0";

	char *AT_NAME = ( char* )"AT+NAME?\n\r";
	char *AT_VERSION = ( char* )"AT+VERSION?\n\r";
	char *AT_ROLE = ( char* )"AT+ROLE?\n\r";
	char *AT_CMODE = ( char* )"AT+CMODE?\n\r";
	char *AT_ADDR = ( char* )"AT+ADDR?\n\r";
	char *AT_BIND = ( char* )"AT+BIND?\n\r";
	char *AT_UART = ( char* )"AT+UART?\n\r";


	//	PUBLIC PROPERTIES AND METHODS
public:
	SoftwareSerial * blueTooth;

	char buffer [ BUFFER_SIZE ] = { 0x00 };
	uint8_t index = 0;


	//	Constructor
	BJHC05Utility () {
		Serial.println ( F ( "Constructor: BJ HC05 Utility" ) );
	}


	/***************************************************************
	*
	*	PERFORM STARTUP OPERATIONS (called from within setup)
	*
	****************************************************************/
	void setup () {
		if (_tracing) {
			Serial.println ( F ( "Initializing HC05 Utility" ) );
		}

		setupPins ();
		setupBluetooth ();
		setupDisplay ();
		setupButtons ();
	}

	void setupPins () {
		pinMode ( POWER_LED_PIN, OUTPUT );
		pinMode ( ATMODE_LED_PIN, OUTPUT );
		pinMode ( STATE_LED_PIN, OUTPUT );
		pinMode ( ENABLED_LED_PIN, OUTPUT );

		pinMode ( MODE_LED_PIN, OUTPUT );
		pinMode ( STATUS_LED_PIN, OUTPUT );

		pinMode ( TX, OUTPUT );
		pinMode ( RX, INPUT );
		pinMode ( ENA, OUTPUT );
		pinMode ( STATE_PIN, INPUT );
		pinMode ( POWER_PIN, OUTPUT );

		flashDance ();

		turnOn ( POWER_PIN );
		turnOn ( POWER_LED_PIN );
	}

	void setupBluetooth () {
		//	Enable connection with the HC05 module
		blueTooth = new SoftwareSerial ( RX, TX );
		blueTooth->begin ( 9600 );
	}

	void setupDisplay () {
		//	Setup display
		Wire.begin ();
		Wire.setClock ( 400000L );

#if RST_PIN >= 0
		oled.begin ( &Adafruit128x64, I2C_ADDRESS, RST_PIN );
#else // RST_PIN >= 0
		oled.begin ( &Adafruit128x64, I2C_ADDRESS );
#endif // RST_PIN >= 0

		//	Pick your font
		oled.setFont ( Adafruit5x7 );
		//oled.setFont(Callibri15);
		//oled.setFont(Arial14);
		//oled.setFont ( Callibri11_bold );
		//oled.setFont ( TimesNewRoman13 );

		// Increase space between letters.
		oled.setLetterSpacing ( 1 );

		oled.clear ();
		oled.set1X ();
	}

	void setupButtons () {
		powerButton = new BJDriverButton ( buttonSwitch1 );
		powerButton->setTracing ( TRACING );
		powerButton->configure ( BUTTON_1_PIN, BUTTON_1_LED, BUTTON_1_MODE, BUTTON_1_STATES );
//		flash ( BUTTON_1_LED );

		modeButton = new BJDriverButton ( buttonSwitch2 );
		modeButton->setTracing ( TRACING );
		modeButton->configure ( BUTTON_2_PIN, BUTTON_2_LED, BUTTON_2_MODE, BUTTON_2_STATES );
//		flash ( BUTTON_2_LED );
		modeButton->setValue ( ( _powerIsOn ) ? 1 : 0 );

		displayButtonNames ();
	}

	void reportMessage ( char* message, int row = -1 ) {
		if (_tracing) {
			Serial.println ( message );
			if (row > 0) {
				showRow ( "ReportMessage1", row );
				oled.setCursor ( 0, row );
				oled.println ( message );
			}
		}
	}

	//	REPORT SOME ERROR
	void reportError ( char *message, int code ) {
		if (_lastErrorNumber == code) {
			return;
		}

		_lastErrorNumber = code;
		Serial.println ( message );
	}

	//	EXIT ON ERROR
	void bail ( char* message ) {
		Serial.println ( message );
		delay ( 1000 );
		exit ( -1 );
	}

	/***************************************************************
	*
	*	BUTTON MANAGEMENT METHODS
	*
	****************************************************************/
	void configureButton ( BJDriverButton *button, int pin ) {
		pinMode ( pin, OUTPUT );
		flash ( pin );
	}

	//	Return true if value changed
	bool processButton ( BJDriverButton *button, int ledPin ) {

		//	Read the button
		button->getButtonState ();

		//	See if it has anything useful for us...
		if (button->getTogglingState () != button->TogglingStateCompleted) {
			//	Apparently not
			return false;
		}

		flash ( ledPin );

		reportButtonState ( button );

		button->reset ();

		return true;
	}

	void reportButtonState ( BJDriverButton *button ) {
		if (button->getTracing ()) {
			showButtonState ( button );
			displayButtonState ( button );
		}
	}

	void showButtonState ( BJDriverButton *button ) {
		Serial.print ( button->getName () );
		Serial.print ( button->getModeName () );
		Serial.print ( F ( " value: [" ) );
		Serial.print ( button->getValue () );
		Serial.println ( F ( "]" ) );
	}

	void displayButtonState ( BJDriverButton *button ) {
		int value = button->getValue ();

		oled.clear ();
		oled.print ( button->getName () );
		oled.println ( button->getModeName () );
		oled.print ( F ( " Value: [" ) );
		oled.print ( value );
		oled.println ( F ( "]" ) );
	}

	void displayButtonNames () {
		oled.clear ();
		oled.print ( F ( BUTTON_1_NAME ) );
		oled.println ( powerButton->getModeName () );

		oled.print ( F ( BUTTON_2_NAME ) );
		oled.println ( modeButton->getModeName () );
	}

	/***************************************************************
	*
	*	PIN MANAGEMENT METHODS
	*
	****************************************************************/

	//	Test each LED to make sure they work
	void flashDance () {
		flash ( POWER_LED_PIN );
		flash ( ATMODE_LED_PIN );
		flash ( ENABLED_LED_PIN );
		flash ( STATE_LED_PIN );
		flash ( MODE_LED_PIN );
		flash ( STATUS_LED_PIN );
	}

	//	Turn pin on and off
	void flash ( int pin, int times = 5, int wait = 20 ) {
		for (int i = 0; i < times; i++) {
			turnOn ( pin );
			delay ( wait );
			turnOff ( pin );
			delay ( wait );
		}
	}

	void showLED ( int pin, bool state ) {
		if (state) turnOn ( pin );
		else turnOff ( pin );
	}

	void turnOn ( char sel ) {
		if (isdigit ( sel )) {
			turnOn ( int ( sel ) );
			return;
		}
	}

	//	Turn pin off
	void turnOff ( char action ) {
		if (isdigit ( action )) {
			turnOff ( int ( action ) );
		}
	}

	//	Turn pin on
	void turnOn ( int pin ) {
		digitalWrite ( pin, HIGH );
	}

	//	Turn pin off
	void turnOff ( int pin ) {
		digitalWrite ( pin, LOW );
	}

	//	Return current pin value
	bool readPin ( int pin ) {
		return ( digitalRead ( pin ) == HIGH ) ? true : false;
	}


	/******************************************************************
	*
	*	PERFORM MAIN LOOP
	*
	*******************************************************************/
	void loop () {

		//	The MODE button is the path into or out of AT Command Mode
		//	See if it has been pressed. 
		if (processButton ( modeButton, BUTTON_2_LED )) {
			Serial.print ( modeButton->getName () );
			Serial.println ( modeButton->getValue () );
			toggleATMode ( powerButton );
			return;
		}

		//	PASSIVE: Report current state - take no action
		if (processButton ( powerButton, BUTTON_1_LED )) {
			Serial.print ( powerButton->getName () );
			Serial.println ( powerButton->getValue () );

			_currentState = getState ();
			reportState ( _currentState );
			return;
		}

		if (getCommand ()) {
			doCommand ();
		}
	}

	bool toggleATMode ( BJDriverButton *button ) {
		int row = 0;
		Serial.println ( F ( "Entering toggleATMode" ) );
		if (button->getValue () == 0) {
			return false;
		}

		oled.clear ();
		oled.setCursor ( 0, row );
		oled.print ( F ( "Toggling AT Mode" ) );
		oled.setCursor ( 0, ++row );
		showRow ( "ToggleATMode1", row );
		row = showPowerValue (row);
		showRow ( "ToggleATMode2", row );
		oled.setCursor ( 0, ++row );
		showRow ( "TottleATMode3", row );
		row = showStateValue (row);

		oled.setCursor ( 0, ++row );
		showRow ( "TottleATMode4", row );
		row = showEnabledValue ( row );
		showRow ( "TottleATMode5", row );

		return _ATCommandModeEnabled;
	}

	void showRow ( char*message, int row ) {
		Serial.print ( message );
		Serial.print ( F ( " Row=" ) );
		Serial.println ( row );
	}

	int showPowerValue ( int row ) {
		showRow ( "showPowerValue1", row );
		bool _powerIsOn = readPowerPin ();
		if (_powerIsOn) {
			_powerIsOn = turnOffPower (row);
		}

		oled.setCursor ( 0, ++row );
		showRow ( "showPowerValue2", row );
		oled.print ( F ( "POWER:" ) );
		oled.setCursor ( 60, row );
		oled.print ( ( _powerIsOn ) ? ( char* )"ON" : ( char* )"OFF" );

		Serial.print ( F ( "POWER:\t" ) );
		Serial.println ( ( _powerIsOn ) ? ( char* )"ON" : ( char* )"OFF" );

		showRow ( "showPowerValue3", row );
		return row;
	}

	int showStateValue (int row) {
		bool _state = readStatePin ();
		oled.setCursor ( 0, row );
		oled.print ( F ( "STATE:" ) );
		oled.setCursor ( 60, row );
		oled.print ( ( _state ) ? ( char* )"ON" : ( char* ) "OFF" );

		Serial.print ( F ( "STATE:\t" ) );
		Serial.println ( ( _state ) ? ( char* )"ON" : ( char* ) "OFF" );
		return row;
	}

	int showEnabledValue ( int row ) {
		bool _enabled = readEnabledPin ();		//	Return true if ENA pin is HIGH
		if (!_powerIsOn && !_enabled) {
			_enabled = enableATMode ();
		}

		oled.setCursor ( 0, ++row );
		oled.print ( "ENA:" );
		oled.setCursor ( 60, row );
		oled.print ( ( _enabled ) ? "ON" : "OFF" );

		//if (!_enabled) {
		//	oled.setCursor ( 0, ++row );
		//	oled.print ( F ( "AT MODE:" ) );
		//	oled.setCursor ( 60, row );
		//	oled.print ( ( _enabled ) ? "SUCCESS" : "FAILED" );
		//	oled.setCursor ( 0, ++row );
		//}

		Serial.print ( "ENA:\t" );
		Serial.println ( ( _enabled ) ? "ON" : "OFF" );

		turnOnPower ();
		return row;
	}

	bool turnOffPower (int row) {
		turnOff ( POWER_LED_PIN );
		turnOff ( POWER_PIN );
		_powerIsOn = false;
		_powerCut = true;
		reportMessage ( ( char* ) "POWER CUT TO HC05", row );
		return _powerIsOn;
	}

	bool turnOnPower () {
		turnOn ( POWER_LED_PIN );
		turnOn ( POWER_PIN );
		_powerIsOn = true;
		_powerCut = false;
		reportMessage ( ( char* ) "POWER ON TO HC05" );
		return _powerIsOn;
	}

	//	Reflect state of ENA and STATE pins on board
	//	Returned value indicates current state:
	//	state && 1 --> Status pin is HIGH
	//	state && 2 --> Enabled pin is HIGH
	//	state && 4 --> Power connected
	//	state && 8 --> AT Mode
	//	state && 16 --> Normal mode
	int getState () {
		int state = 0;

		bool _state = readStatePin ();
		if (_state) {
			state += 0x01;
		}

		bool _enabled = readEnabledPin ();		//	Return true if ENA pin is HIGH

		bool _power = readPowerPin ();

		if (_enabled) {
			state += 0x02;
		}

		if (_power) {
			state += 0x04;
		}

		if (_ATCommandModeEnabled) {
			state += 0X08;
		}

		return state;
	}

	void reportState ( int state ) {
		showState ( state );
		displayState ( state );
	}

	//	Returned value indicates current state:
	//	state & 1 --> Status pin is HIGH
	//	state & 2 --> Enabled pin is HIGH
	//	state & 4 --> Power connected
	//	state & 8 --> AT Mode
	//	state & 16 --> Normal mode
	int showState ( int state ) {
		if (state != _lastState) {
			Serial.print ( F ( "\nCurrent State:\t" ) );
			Serial.println ( state );

			Serial.print ( "Status:\t\t" );
			Serial.println ( ( state & 0x01 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

			Serial.print ( "Enable:\t\t" );
			Serial.println ( ( state & 0x02 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

			Serial.print ( "Power:\t\t" );
			Serial.println ( ( state & 0x04 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

			Serial.print ( "AT Ready Mode:\t" );
			Serial.println ( ( state & 0x08 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

			Serial.print ( "Command Mode:\t" );
			Serial.println ( ( state & 0x10 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

			Serial.print ( "Normal Mode:\t" );
			Serial.println ( ( state & 0x20 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );
		}

		_lastState = state;

		return state;
	}

	void displayState ( int state ) {
		oled.clear ();
		oled.setCursor ( 0, 0 );
		oled.print ( F ( "CURRENT STATUS: " ) );
		oled.print ( state, HEX );
		oled.println ();

		oled.setCursor ( 0, 2 );
		oled.print ( "STATUS: " );
		oled.setCursor ( 60, 2 );
		oled.print ( ( state & 0x01 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

		oled.setCursor ( 0, 3 );
		oled.print ( "ENA: " );
		oled.setCursor ( 60, 3 );
		oled.print ( ( state & 0x02 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

		oled.setCursor ( 0, 4 );
		oled.print ( "POWER: " );
		oled.setCursor ( 60, 4 );
		oled.print ( ( state & 0x04 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

		oled.setCursor ( 0, 5 );
		oled.print ( "READY:" );
		oled.setCursor ( 60, 5 );
		oled.print ( ( state & 0x08 ) ? ( char* ) "HIGH" : ( char* ) "LOW" );

		oled.setCursor ( 0, 6 );
		oled.print ( "Mode: " );
		oled.setCursor ( 60, 6 );
		oled.print ( ( state & 0x10 ) ? ( char* ) "AT" : ( char* ) "NORMAL" );
	}

	//	Return true if STATE pin is HIGH
	bool readStatePin () {
		int pinVal = readPin ( STATE_PIN );
		if (pinVal) {
			turnOn ( STATE_LED_PIN );
		} else {
			turnOff ( STATE_LED_PIN );
		}
		return pinVal;
	}

	//	Return true if ENA pin is HIGH
	bool readEnabledPin () {
		pinMode ( ENA, INPUT );
		//	Read the ENA pin
		bool enabled = readPin ( ENA );
		if (enabled) {
			turnOn ( ENABLED_LED_PIN );
		} else {
			turnOff ( ENABLED_LED_PIN );
		}
		pinMode ( ENA, OUTPUT );
		return enabled;
	}

	bool readPowerPin () {
		bool power = readPin ( POWER_PIN );
		if (power) {
			turnOn ( POWER_LED_PIN );
		} else {
			turnOff ( POWER_LED_PIN );
		}
		return power;
	}

	bool enableATMode () {
		//	Disable bluetooth
		blueTooth->end ();
		delay ( BT_WAIT );

		//	Reconnect at AT Command Mode baud rate
		blueTooth->begin ( 38400 );

		Serial.println ( "Entering AT Mode..." );

		//	Attempt co connect
		//Serial.print ( F ( "Sending " ) );
		//Serial.print ( AT );

		_ATCommandModeEnabled = sendReceive ( AT );
		Serial.print ( F ( "Bluetooth Says: " ) );

		if (_ATCommandModeEnabled) {
			Serial.println ( F ( "SUCCESS" ) );
		} else {
			blueTooth->end ();
			delay ( BT_WAIT );
			blueTooth->begin ( 9600 );
			Serial.println ( F ( "FAILURE" ) );
		}
		return _ATCommandModeEnabled;
	}

	bool readModule () {
		bool OK = true;
		if (sendReceive ( AT_NAME )) {
			oled.print ( F ( "NAME:" ) );
			oled.println ( _response );
		} else OK = false;

		if (sendReceive ( AT_ROLE )) {
			oled.print ( F ( "ROLE:" ) );
			oled.println ( _response );
		} else OK = false;

		if (sendReceive ( AT_CMODE )) {
			oled.print ( F ( "CMODE:" ) );
			oled.println ( _response );
		} else OK = false;

		if (sendReceive ( AT_ADDR )) {
			oled.print ( F ( "ADDR:" ) );
			oled.println ( _response );
		} else OK = false;

		if (sendReceive ( AT_BIND )) {
			oled.print ( F ( "CBIND:" ) );
			oled.println ( _response );
		} else OK = false;

		if (sendReceive ( AT_UART )) {
			oled.print ( F ( "UART:" ) );
			oled.println ( _response );
		} else OK = false;

		return OK;
	}

	/******************************************************************
	*
	*	SEND / RECEIVE METHODS
	*
	*******************************************************************/
	//bool sendCommand () {
	//	return false;
	//}

	bool sendReceive ( char* command ) {
		if (_tracing) {
			Serial.print ( F ( "SENDING [" ) );
			Serial.print ( command );
			Serial.println ( F ( "]" ) );
		}

		int bytesWritten = blueTooth->write ( command );
		if (bytesWritten == 0) {
			Serial.println ( F ( "FAILED TO SEND" ) );
			return false;
		}

		delay ( BT_WAIT );
		if (blueTooth->available ()) {
			_responseIndex = 0;
			while (blueTooth->available ()) {
				_response [ _responseIndex++ ] = blueTooth->read ();
				_response [ _responseIndex ] = 0;
			}
		} else {
			Serial.println ( F ( "NO RESPONSE" ) );
			return false;
		}

		Serial.print ( F ( "RESPONSE: [" ) );
		Serial.print ( _response );
		Serial.println ( F ( "]" ) );

		return true;
	}


	// the setup function runs once when you press reset or power the board
	//	Read data from Serial port and write it to the Bluetooth port
	int sendData () {
		int dataSent = 0;
		if (Serial.available ()) {
			while (Serial.available ()) {
				char data = Serial.read ();
				Serial.write ( data );
				if (blueTooth->write ( data ) < 1) {
					//	No data sent, in spite of our best efforts
					Serial.print ( "*" );
				} else {
					++dataSent;
				}
			}
		}

		if (dataSent > 0) {
			Serial.print ( dataSent );
			Serial.println ( F ( " bytes sent" ) );
		}

		return dataSent;
	}

	int getData () {
		int dataReceived = 0;
		blueTooth->listen ();
		if (blueTooth->available ()) {
			while (blueTooth->available ()) {
				char data = ( char ) blueTooth->read ();
				Serial.write ( data );
				++dataReceived;
			}
		}

		if (dataReceived > 0) {
			Serial.print ( dataReceived );
			Serial.println ( " bytes received" );
		}

		return dataReceived;
	}


	/******************************************************************
	*
	*	COMMAND HANDLING METHODS
	*
	*******************************************************************/
	//	Check Serial Input for any input. 
	//	If it begins with "AT", then send it to the 
	bool getCommand () {
		if (!Serial.available ()) return false;
		while (Serial.available ()) {
			uint8_t data = Serial.read ();
			if (( index < BUFFER_SIZE ) && data > 0x20) {
				buffer [ index++ ] = data;
			}
		}
		return true;
	}


	void doCommand () {
		dumpString ( buffer, index );
		char dom = tolower ( buffer [ 0 ] );
		char sel = tolower ( buffer [ 1 ] );
		char act = tolower ( buffer [ 2 ] );

		if (_tracing) {
			Serial.print ( F ( "DOM:[" ) );
			Serial.print ( dom );
			Serial.print ( F ( "] SEL:[" ) );
			Serial.print ( sel );
			Serial.print ( F ( "] ACT:[" ) );
			Serial.print ( act );
			Serial.println ( F ( "]" ) );
		}

		switch (dom) {

		case '?':
			if (sel == '*') {
				showDomainList ();
			} else if (sel == '?') {
				showHelp ();
			} else if (sel == '!') {
				showModuleProperties ();
			}
			break;

		case '!':
			if (sel == 'a' && act == 't') {
				buffer [ 0 ] = 0x00;
				compressBuffer ();
				sendReceive ( buffer );
				clearBuffer ();
				break;
			}

			if (sel == '?') {
				showATCommands ();
				break;
			}

			break;


		case 'p':
			//	Turn pin on or off
			switch (act) {
			case '+':
				turnOn ( sel );
				break;
			case '-':
				turnOff ( sel );
				break;
			}

		case 't':
			switch (sel) {
			case '?':
				showTracingCommands ();
				break;

			case '+':
				_tracing = true;
				powerButton->setTracing ( true );
				modeButton->setTracing ( true );
				break;

			case '-':
				_tracing = false;
				powerButton->setTracing ( false );
				modeButton->setTracing ( false );
				break;
			}
			break;

		case 'b':
			Serial.print ( F ( "Button cmd:..." ) );
			if (sel == '?') {
				showButtonCommands ();
				break;
			}

			switch (act) {
			case '+':
				turnOn ( sel );
				break;
			case '-':
				turnOff ( sel );
				break;
			case '?':
				break;
			}
			break;
		}

		clearBuffer ();
	}


	/******************************************************************
	*
	*	AT COMMAND HANDLING METHODS
	*
	*******************************************************************/
	void showATCommands () {
		Serial.println ( F ( "AT Query Commands:" ) );
		Serial.print ( AT_NAME );
		Serial.print ( AT_VERSION );
		Serial.print ( AT_ROLE );
		Serial.print ( AT_CMODE );
		Serial.print ( AT_ADDR );
		Serial.print ( AT_BIND );
		Serial.print ( AT_UART );
	}

	void showModuleProperties () {
		sendReceive ( AT_NAME );
		sendReceive ( AT_VERSION );
		sendReceive ( AT_ROLE );
		sendReceive ( AT_CMODE );
		sendReceive ( AT_ADDR );
		sendReceive ( AT_BIND );
		sendReceive ( AT_UART );
	}

	/******************************************************************
	*
	*	HELP METHODS
	*
	*******************************************************************/
	void showHelp () {
		Serial.println ( F ( "Commands have several parts:" ) );
		Serial.println ( F ( "<dom> - The domain that the command applies to" ) );
		Serial.println ( F ( "<sel> - the optional selector or filter that narrows the domain" ) );
		Serial.println ( F ( "<act> - the action to be performed" ) );
		Serial.println ( F ( "? will show this list of commands" ) );
		Serial.println ( F ( "?* will show a list of domains" ) );
		Serial.println ( F ( "<dom>? will show commands that apply to that domain" ) );
	}

	void showDomainList () {
		Serial.println ( F ( "Each domain is identified by a single character <dom>" ) );
		Serial.println ( F ( "! = AT Commands" ) );
		Serial.println ( F ( "  -everything past the '!' is passed to the HC05" ) );
		Serial.println ( F ( "B = Button Commands" ) );
		Serial.println ( F ( "T = Tracing- Enter AT<command>" ) );
	}

	void showTracingCommands () {
		Serial.print ( F ( "Tracing Commands:" ) );
		Serial.println ( F ( "T<sel><act> WHERE:" ) );
		Serial.println ( F ( "<sel> = button number or * for 'ALL'" ) );
		Serial.println ( F ( "<act> = + to turn on, - to turn off or ? to query" ) );
		Serial.println ( F ( "Examples:" ) );
		Serial.println ( F ( "T1+ = enable tracing on button 1" ) );
		Serial.println ( F ( "T2- = disable tracing on button 2" ) );
		Serial.println ( F ( "T*+ = turn on tracing on all buttons" ) );
		Serial.println ( F ( "T*? = Show tracing state for all buttons " ) );
	}

	void showButtonCommands () {
		Serial.println ( F ( "Button Commands:" ) );
		Serial.print ( F ( "!b*+" ) );
	}


	/******************************************************************
	*
	*	BUFFER MANAGEMENT METHODS
	*
	*******************************************************************/
	void showBuffer ( char* label ) {
		Serial.print ( label );
		Serial.print ( F ( ": [" ) );
		Serial.print ( buffer );
		Serial.println ( F ( "]" ) );
	}

	void clearBuffer () {
		for (int i = 0; i < BUFFER_SIZE; i++) {
			buffer [ i ] = 0x00;
		}
		index = 0;
	}

	void compressBuffer () {
		int from = 0;
		int to = 0;

		//	Skip past empty space but don't fall off the edge of the world
		while (from < BUFFER_SIZE && buffer [ from ] == 0x00) ++from;

		//	Slide each character towards the begining of the buffer
		while (to < BUFFER_SIZE - from) {
			buffer [ to++ ] = buffer [ from++ ];
		}

		//	Set the rest of the buffer to 0x00
		while (from < BUFFER_SIZE) {
			buffer [ from++ ] = 0x00;
		}
	}

	bool sendBuffer ( bool echoBuffer ) {
		if (echoBuffer) {
			//	Display the contents of the command buffer
			Serial.print ( F ( "Sending: [" ) );
			Serial.print ( buffer );
			Serial.println ( F ( "]" ) );
		}

		int sent = blueTooth->write ( buffer );

		//	Wait for a brief moment before checking for a response
		delay ( BT_WAIT );
		clearBuffer ();
		return ( sent > 0 );
	}


	/***************************************************************
	*
	*	UTLITY METHODS
	*
	****************************************************************/

	void dumpString ( char* buffer, unsigned int size ) {
		if (!_tracing) return;

		unsigned int col0 = 0;
		char letters [ COLS + 1 ] = {};
		for (unsigned int i = 1; i <= size; i++) {
			printHexByte ( buffer [ i - 1 ] );
			if (( i >= COLS && i % COLS == 0 ) || ( i > size - 1 )) {
				Serial.print ( '\t' );
				unsigned int k = 0;
				for (unsigned int j = col0; j < col0 + COLS; j++) {
					letters [ k++ ] = ( buffer [ j ] < 0x20 ) ? '.' : ( char ) buffer [ j ];
				}
				letters [ COLS ] = 0;
				col0 = i + 1;
				Serial.println ( letters );
			} else {
				Serial.print ( ' ' );
			}
		}
		Serial.println ();
	}

	void printHexByte ( byte b ) {
		if (b < 0X10) Serial.print ( '0' );
		Serial.print ( b, HEX );
		Serial.print ( ' ' );
	}
};
#endif

