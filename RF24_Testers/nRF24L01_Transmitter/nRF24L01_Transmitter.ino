/*****************************************************************************
*
*	nRF24L01 - Transmitter Version 4.0.0 - Struct (multi-message)
*
*	Transmit a large message in the form of a struct using multiple subpackets
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
#define DATA_BUFFER_SIZE 60	//	Serial buffer size is 64
#define LOOP_DELAY 100
#define ACK false
#define SHOW_MARKERS true

bool _tracing = true;

//	Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 11, 12, 13) 
//	plus pins 9 & 10 which can be located elsewhere
RF24 radio(14,15);

//	This is just the way the RF24 library works:
//byte addresses[][6] = {
//	{ 'R', 'E', 'M', 'O', 'T' },	//	Transmitter
//	{ 'R', 'O', 'B', 'O', 'T' }		//	Receiver
//};
byte transmitter [6] = { 'R', 'E', 'M', 'O', 'T' };
byte receiver [6] = { 'R', 'O', 'B', 'O', 'T' };

bool listening = false;
byte * listeningAddress;

#define COMMAND_SIZE	32 - sizeof(int) -sizeof(int) - sizeof(uint8_t)

//	Data structure that will be passed back and forth between
//	Client and server.
struct dataStruct {
	unsigned int packetNumber;			//	Used to find missing packets
	unsigned int length;				//	Length in bytes of entire command
	byte subPacket;						//	Sub-packet #
	char command[COMMAND_SIZE] = {};	//	Partial command string to be executed
} commandData;

unsigned int packetNumber = 0;
byte subPacket = 0;
char dataBuffer[DATA_BUFFER_SIZE] = {};
unsigned int dataSent = 0;
bool prevStatus = false;

// -----------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// -----------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.print ( "\nTRANSMITTER Version " );
	Serial.print ( VERSION );
	Serial.println ( F ( " Multi - part Struct" ));

#if LED_PIN > 0
	pinMode ( LED_PIN, OUTPUT );
#endif

	// Initiate the radio object
	radio.begin();

	// Set the transmit power to lowest available to prevent power supply related issues
	radio.setPALevel(RF24_PA_LOW);

	// Set the speed of the transmission to the quickest available
	radio.setDataRate(RF24_2MBPS);

	// Use a channel unlikely to be used by Wifi, Microwave ovens etc
	radio.setChannel(124);

	// Open a writing and reading pipe on each radio, with opposite addresses
	setWritingPipe ( transmitter );
	setReadingPipe ( receiver );
	listeningAddress = receiver;

	showInfo ();

	prepareCommandData();

	Serial.println ( "ENTER STRING IN SERIAL MONITOR:" );
}

void setWritingPipe ( byte address [6] ) {
	showAddress ( "Writing Pipe", address );
	radio.openWritingPipe ( address );
}

void setReadingPipe ( byte address [6] ) {
	showAddress ( "Reading Pipe", address );
	radio.openReadingPipe ( 1, address );
}

void startListening () {
	if (listening) return;

	listening = true;
//	radio.openReadingPipe ( 1, listeningAddress );
	radio.startListening ();
	showAddress ( "STARTED LISTENING ON ", listeningAddress );

	if (radio.available ()) {
		Serial.println ( F ( "DATA AVAILABLE" ) );
	}
}

void stopListening () {
	if (listening) {
		listening = false;
	}
	radio.stopListening ();
	showAddress ( "STOPPED LISTENING ON ", listeningAddress );
}

// -----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -----------------------------------------------------------------------------
void loop() {
	if (getDataToSend()) {
		sendDataBuffer ();
	} else {
		Serial.flush ();
	}

	clearDataBuffer ();

	showStatusLED ();

	// Try again 1s later
	delay(LOOP_DELAY);
}

//	Read data from the serial monitor into the data buffer
bool getDataToSend() {
	if (!Serial.available()) return false;

	clearDataBuffer();
	byte data;
	unsigned int index = 0;
	while (Serial.available()) {
		data = Serial.read();
		if (data < 0x20) continue;
		if (index < (DATA_BUFFER_SIZE - 1)) {
			dataBuffer[index++] = data;
			dataBuffer[index] = 0x00;
		} else {
			Serial.println ( F ( "\n*** TOO MUCH DATA ***" ) );
			dumpString ( dataBuffer, DATA_BUFFER_SIZE );
			clearDataBuffer ();
			return false;
		}
		delay(1);
	}

	if (strlen ( dataBuffer ) == 0) return false;

	//if (_tracing) {
	//	Serial.print ( F ( "\nSending " ) );
	//	Serial.print ( strlen ( dataBuffer ) );
	//	Serial.print ( F ( " bytes in " ) );
	//	showDataBuffer ( "" );
	//	delay ( 10 );
	//}
	return true;
}

void sendDataBuffer() {
	// Ensure we have stopped listening (even if we're not) or we won't be able to transmit

	prepareCommandData();
	stopListening ();
	while (subPacketReady()) {

		if (!radio.write(&commandData, sizeof(commandData))) {
			showCommand ((String)"Write failed");
		} else {
			showCommand((String)"Data Sent: ");
		}

#if ACK
		// Now listen for a response
		startListening();
		// But we won't listen for long, 200 milliseconds is enough
		unsigned long started_waiting_at = millis();

		// Loop here until we get indication that some data is ready for us to read (or we time out)
		while (!radio.available()) {

			// Oh dear, no response received within our timescale
			if (millis() - started_waiting_at > 500) {
				Serial.println("No response received - timeout!");
				return;
			}
		}

		// Now read the data that is waiting for us in the nRF24L01's buffer
		radio.read(&commandData, sizeof(commandData));
		showCommand((String)"Data Received: ");
		stopListening ();
#endif
	}


	//	Get ready for next command
	prepareCommandData();
	startListening ();
}

//	If the data to be sent is larger than the size of
//	commandData.command, then break the data into packets
bool subPacketReady() {
	//	see if data buffer is empty
	if (strlen(dataBuffer) == 0) {
		return false;
	}

	commandData.packetNumber = ++packetNumber;
	commandData.subPacket = 1;

	//	If contents of data buffer will fit into one packet
	//	then send it and clear the data buffer.
	if (strlen(dataBuffer) < sizeof(commandData.command)) {
		strcpy(commandData.command, dataBuffer);
		commandData.length = strlen(commandData.command);
		commandData.subPacket = 0xFF;
		clearDataBuffer();
		return true;
	}

	//	Get command data
	for (unsigned int i = 0; i < sizeof(commandData.command) - 1; i++) {
		commandData.command[i] = dataBuffer[i];
		commandData.command[i + 1] = 0x00; // Terminate the string
	}
	
	commandData.length = strlen(commandData.command);
	dataSent = strlen(commandData.command);

	//	Compress the data buffer by sliding bytes to the
	//	beginning of the buffer
	for (unsigned int i=0; i<sizeof(dataBuffer); i++) {
		dataBuffer[i] = dataBuffer[dataSent++];
		if (dataSent > sizeof(dataBuffer)) break;
	}

	commandData.subPacket = ++subPacket;

	return true;
}

void clearDataBuffer() {
	for (unsigned int i = 0; i < sizeof(dataBuffer); i++) {
		dataBuffer[i] = 0x00;
	}
}

void showDataBuffer(String label) {
	if (!_tracing) return;

	Serial.print("dataBuffer: ");
	Serial.print(label);
	Serial.print ( F ( " [" ) );
	Serial.print ( dataBuffer );
	Serial.println ( F ( "]" ) );
	//	dumpString(dataBuffer, sizeof(dataBuffer));
}

void clearCommandData() {
	//	clear the command 
	for (unsigned int i = 0; i < sizeof(commandData.command); i++) {
		commandData.command[i] = 0x00;
	}
}

void prepareCommandData() {
	clearCommandData();
	commandData.length = 0;
	commandData.subPacket = 0;
	commandData.packetNumber = 0;

	subPacket = 0;
}

void showCommandData() {
	if (!_tracing) return;

	// Show user what we sent and what we got back
	Serial.print("Packet #:");
	Serial.print(commandData.packetNumber);
	Serial.print(", subPacket#:");
	Serial.print(commandData.subPacket);
	Serial.print(", length:");
	Serial.print(commandData.length);
	Serial.print("\ndata: [");
	Serial.print(commandData.command);
	Serial.println("]");
}

void showCommand(String label) {
	if (!_tracing) return;

	// Show user what we sent and what we got back
	Serial.print(label);
	Serial.print(F(" packet #: "));
	Serial.print(commandData.packetNumber);
	Serial.print(F(", subPacket#:"));
	Serial.print(commandData.subPacket);
	Serial.print ( F ( ", Length: " ) );
	Serial.print ( commandData.length );
	Serial.print(F(", data:\n["));
	Serial.print ( commandData.command );
	Serial.println ( F ( "]" ) );
}

void dumpString(String buffer, unsigned int size) {
	if (!_tracing) return;

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
		radio.begin ();								// Attempt to re-configure the radio with defaults
		radio.failureDetected = 0;					// Reset the detection value
		radio.openWritingPipe ( receiver );			// Re-configure pipe addresses
		radio.openReadingPipe ( 1, transmitter );

		Serial.println ( F ( "*** Failure detected ***" ) );
		return false;
	}

	if (_tracing) {
		Serial.println ( "Radio Status OK" );
	}
	return true;
}

void showStatusLED () {
#if LED_PIN > 0
	bool status = radio.isValid () && !radio.failureDetected;
	if (prevStatus != status) {
		prevStatus = status;
		digitalWrite ( LED_PIN, ( status )?HIGH:LOW );
		Serial.print ( F ( "Radio Status: " ) );
		Serial.println ( ( status )?( String )"OK":( String ) "NOT WORKING" );
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
	Serial.println ( F ( "ACK Disabled" ) );
#endif

	checkRadioStatus ();
}

void showAddresses () {
	showAddress ( "Transmitter", transmitter );
	showAddress ( "Receiver", receiver );
}

void showAddress ( String role, byte address [6] ) {
	Serial.print ( role );
	Serial.print ( F ( ": [" ) );
	for (unsigned int i = 0; i < 6; i++) {
		if (address [i] > 0x20) {
			Serial.print ( (char) address [i] );
		}
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


