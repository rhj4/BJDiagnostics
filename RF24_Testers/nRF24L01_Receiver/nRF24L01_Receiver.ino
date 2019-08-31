/*****************************************************************************
*
*	nRF24L01 - Receiver Version 4.0.0  - Struct (multi-message)
*
*	Receive a large message in the form of a struct using multiple subpackets
*
*	Author: (original: unknown)
*			Current: Bob Jones
*
*	Last update on 8/17/18 by BJ: implemented long commands (up to 60 bytes).
*
*/
#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

#define VERSION "4.0.1"

#define COLS 10
#define LED_PIN 4
#define MESSAGE_SIZE 100
#define ACK true
#define SHOW_MARKERS true

#define WIFI_POWER_LEVEL		RF24_PA_LOW		

//	We don't need lots of speed
#define WIFI_DATA_RATE			RF24_2MBPS			

//	Unlikely to be used by WiFi, Microwave ovens, etc
#define WIFI_DATA_CHANNEL		124					

bool _tracing = true;

//	Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 11, 12, 13) 
//	plus pins 9 & 10 which can be located elsewhere
RF24 radio(14,15);

#define COMMAND_SIZE	32 - sizeof(int) -sizeof(int) - sizeof(uint8_t)

//	This is just the way the RF24 library works:
//byte addresses[][6] = {
//	{ 'W', 'R', 'I', 'T', 'R' },	//	Transmitter
//	{ 'R', 'E', 'A', 'D', 'R' }		//	Receiver
//};

byte transmitter [6] = { 'R', 'E', 'M', 'O', 'T' };
byte receiver [6] = { 'R', 'O', 'B', 'O', 'T' };

bool listening = false;
byte * listeningAddress;

//	Data structure that will be passed back and forth between
//	Client and server.
struct dataStruct {
	unsigned int packetNumber;			//	Used to find missing packets
	unsigned int length;				//	Length in bytes of command
	byte subPacket;						//	Sub-packet #
	char command[COMMAND_SIZE] = {};	//	Command string to be executed
} commandData;

bool radioOK = false;

unsigned long lastPacketNumber = 0;

char message[MESSAGE_SIZE] = {};

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.print ( "\nRECEIVER Version " );
	Serial.print ( VERSION );
	Serial.println(F(" Multi - part Struct"));

#if LED_PIN > 0
	pinMode ( LED_PIN, OUTPUT );
#endif

	// Initiate the radio object
	radio.begin();
	// Set the transmit power to lowest available to prevent power supply related issues
	Serial.print ( F ( "WIFI_POWER_LEVEL=" ) );
	Serial.println ( WIFI_POWER_LEVEL );
	Serial.print ( F ( "WIFI_DATA_RATE=" ) );
	Serial.println ( WIFI_DATA_RATE );
	Serial.print ( F ( "WIFI_DATA_CHANNEL=" ) );
	Serial.println ( WIFI_DATA_CHANNEL );

	radio.setPALevel ( WIFI_POWER_LEVEL );
	Serial.print ( F ( "Radio PA L1evel set to: " ) );
	Serial.println ( radio.getPALevel () );

	// Set the speed of the transmission 
	radio.setDataRate ( WIFI_DATA_RATE );
	Serial.print ( F ( "Radio Data Rate set to: " ) );
	Serial.println ( radio.getDataRate () );

	// Use a channel unlikely to be used by Wifi, Microwave ovens etc
	radio.setChannel ( WIFI_DATA_CHANNEL );
	Serial.print ( F ( "Radio Channel set to: " ) ); Serial.println ( radio.getChannel () );

	// Open a writing and reading pipe on each radio, with opposite addresses
	setWritingPipe ( receiver );
	setReadingPipe ( transmitter );
	listeningAddress = transmitter;

	showInfo ();

	if (checkRadioStatus ()) {
		showStatusLED ();
	}

	clearMessage ();
	Serial.print ( F ( "TRACING IS " ) );
	Serial.println ( ( _tracing )?"ENABLED":"DISABLED" );

	clearCommandData ();
	clearMessage ();

	// Start the radio listening for data
	startListening ();
}

void setWritingPipe ( byte address [6] ) {
	showAddress ( "Writing Pipe", address );
	radio.openWritingPipe ( address );
}

void setReadingPipe ( byte address [6] ) {
	showAddress ( "Reading Pipe", address );
	radio.openReadingPipe ( 1, address );
}

void startListening ( ) {
	if (listening) return;

	listening = true;
	radio.startListening ();
	showAddress ( "STARTED LISTENING ON ", listeningAddress );

	if (radio.available ()) {
		Serial.println ( F ( "DATA AVAILABLE" ) );
	}
}

void stopListening ( ) {
	if (listening) {
		listening = false;
	}
	radio.stopListening ();
	showAddress ( "STOPPED LISTENING ON ", listeningAddress );
}


// -----------------------------------------------------------------------------
// We are LISTENING on this device only (although we do transmit a response)
// -----------------------------------------------------------------------------
void loop() {

	// Is there any data for us to get?
	if (!radio.available()) {
		delay(100);
		return;
	}

	// Go and read the data and put it into that variable
	while (radio.available ()) {
		radio.read ( &commandData, sizeof ( commandData ) );
		int len = strlen ( commandData.command );
		Serial.print ( len );
		if ( len == 0) {
			break;
		}

		if (_tracing) {
			showCommand ( "\nReceived: " );
		}

		//	Check for problems in the packet
		validatePacket ();

		//	Assemble the full message
		if (!appendToMessage ( commandData.command )) {
			return;
		}
		if (_tracing) {
			showCommand ( "Appending" );
		}

		if (commandData.subPacket == 0xff) {
			clearMessage ();
		}

		showCommand ( "Packet: ", true );
		clearCommandData ();
#if ACK
		// First, stop listening so we can talk
		stopListening ();

		//	Acknowledge command
		strcpy ( commandData.command, "ACK" );
		commandData.subPacket = 0x00;	//	Indicate acknowledgement
		commandData.length = strlen ( commandData.command );

		radio.write ( &commandData, sizeof ( commandData ) );

		// Now, resume listening so we catch the next packets.
		startListening ();

		if (_tracing) {
			Serial.print ( "Sent response [" );
			Serial.print ( commandData.command );
			Serial.println ( F ( "]" ) );
		}
#endif
	}

	if (strlen ( message ) > 0) {
		showMessage ();
	}

	showStatusLED ();
}

void validatePacket () {
	if (strlen ( commandData.command ) == 0) return;

	if (strlen ( commandData.command ) != commandData.length) {
		Serial.print ( "\nERROR: commandData.length: " );
		Serial.print ( commandData.length );
		Serial.print ( ", strlen:" );
		Serial.println ( strlen ( commandData.command ) );
	}

	if ( lastPacketNumber + 1 != commandData.packetNumber ) {
		Serial.print ( "\nEXPECTING packet# " );
		Serial.print ( lastPacketNumber + 1 );
		Serial.print ( ", Received #" );
		Serial.println ( commandData.packetNumber );
	}

	lastPacketNumber = commandData.packetNumber;
}

void showMessage () {
	Serial.print ( F ( "Message (" ) );
	Serial.print ( strlen ( message ) );
	Serial.print ( F ( ") [" ) );
	Serial.print ( message );
	Serial.println ( F ( "]" ) );
}

void clearCommandData() {
	//	clear the command 
	for (unsigned int i = 0; i < sizeof(commandData.command); i++) {
		commandData.command[i] = 0x00;
	}
}

bool appendToMessage(char* data) {
	unsigned int len = strlen ( data );
	if (len == 0) {
		return false;
	}

	unsigned int index = strlen ( message );

	for (unsigned int i = 0; i <len; i++) {
		if (strlen ( message ) < MESSAGE_SIZE) {
			message [index++] = data [i];
			message [index] = 0x00;
		} else {
			Serial.print ( F ( "*** MESSAGE BUFER OVERFLOW ***" ) );
			return false;
		}
	}
	return true;
}

void clearMessage() {
	for (unsigned int i = 0; i < sizeof(message); i++) {
		message[i] = 0x00;
	}
}

void showCommand ( String label ) {
	showCommand ( label, false );
}

void showCommand(String label, bool showData) {
	// Show user what we sent and what we got back
	Serial.print(label);
	Serial.print(" packet #: ");
	Serial.print ( commandData.packetNumber );
	Serial.print(", subPacket#:");
	printHexByte(commandData.subPacket);
	if (showData) {
		Serial.print ( "data:[" );
		Serial.print ( commandData.command );
		Serial.println ( F ( "]" ) );
	} else {
		Serial.println ();
	}
}

void dumpString(String buffer, unsigned int size) {
	unsigned int col0 = 0;
	char letters[COLS + 1] = {};
	for (unsigned int i = 1; i <= size; i++) {
		printHexByte(buffer[i - 1]);
		if ((i >= COLS && i % COLS == 0) || (i > size - 1)) {
			Serial.print('\t');
			unsigned int k = 0;
			for (unsigned int j = col0; j < col0 + COLS; j++) {
				letters[k++] = (buffer[j] < 0x20) ? '.' : (char)buffer[j];
			}
			letters[COLS] = 0;
			col0 = i + 1;
			Serial.println(letters);
		}
		else {
			Serial.print(' ');
		}
	}
	Serial.println();
}

void printHexByte(byte b) {
	if (b < 0X10) Serial.print('0');
	Serial.print(b, HEX);
	Serial.print(' ');
}

bool checkRadioStatus () {
	if (!radio.isValid ()) {
		Serial.println ( F ( "*** Radio is NOT VALID ***" ) );
		return false;
	}

	if (radio.failureDetected) {
		radio.begin ();                       // Attempt to re-configure the radio with defaults
		radio.failureDetected = 0;           // Reset the detection value
		radio.openWritingPipe ( transmitter ); // Re-configure pipe addresses
		radio.openReadingPipe ( 1, receiver );

		Serial.println ( F ( "*** Failure detected ***" ) );
		return false;
	}

	return true;
}

void showStatusLED () {
#if LED_PIN > 0
	bool OK = radio.isValid () && !radio.failureDetected;
	if (OK != radioOK) {
		radioOK = OK;
		digitalWrite ( LED_PIN, ( OK )?HIGH:LOW );
		Serial.print ( F ( "Radio Status: " ) );
		Serial.println ( ( OK )?"OK":"NOT WORKING" );
	}
#endif
}

void showInfo () {
	Serial.println ( F ( " RADIO CONFIGURATION:" ) );

	Serial.print ( F ( "Power Level: " ) );
	Serial.println ( radio.getPALevel () );

	Serial.print ( F ( "Data Rate: " ) );
	Serial.println ( radio.getDataRate () );

	Serial.print ( F ( "Channel: " ) );
	Serial.println ( radio.getChannel () );

	Serial.print ( F ( "Payload Size: " ) );
	Serial.println ( radio.getPayloadSize () );

	showAddresses ();

#if ACK
	Serial.println ( F ( "ACK Enabled" ) );
#else
	Serial.println( F ( "ACK Disabled" ) );
#endif

	checkRadioStatus ();
}

void showAddresses () {
	showAddress( "Transmitter", transmitter );
	showAddress ( "Receiver", receiver );
}

void showAddress ( char* role, byte address[6]) {
	Serial.print ( role );
	Serial.print ( F ( ": [" ) );
	for (unsigned int i = 0; i < 6; i++) {
		Serial.print ( (char)address [i] );
	}
	Serial.println ( F ( "]" ) );
}


#ifndef _MARKER_TRACING_
#define _MARKER_TRACING_
static bool _markerTracing = SHOW_MARKERS;

void mark ( char marker ) {
#if SHOW_MARKERS
	if (_markerTracing) {
		Serial.print ( marker );
	}
#endif
}

void markl ( char marker ) {
#if SHOW_MARKERS
	if (_markerTracing) {
		mark ( marker );
		Serial.println ();
	}
#endif
}

void markl () {
#if SHOW_MARKERS
	Serial.println ();
#endif
}

void mark ( char* marker ) {
#if SHOW_MARKERS
	if (_markerTracing) {
		Serial.print ( marker );
	}
#endif
}

void markl ( char* marker ) {
#if SHOW_MARKERS
	mark ( marker );
	markl ();
#endif
}
#endif
