/********************************************************************
*
*	Serial Wired Tester
*
*	Test program to check out Communications between two computers
*	that are directly wired together
*
*	Author: Bob Jones
*	Date:	December 15, 2018
*
*
*********************************************************************/

#include <SoftwareSerial.h>
#define ROLE_IS_SENDER		true
#define ROLE_IS_RECEIVER	false

#define FLASH_TIMES			10
#define FLASH_DURATION		50

#define SENDER_LED			A3
#define RECEIVER_LED		A0

//	If > 0, use that value; otherwise, ask for pin #
#define TX					8
#define RX					7

#define SendingPort			TX
#define ReceivingPort		RX

#define ECHO				false

SoftwareSerial *transporter;
char *name = (ROLE_IS_RECEIVER) ? "Receiver" : "Sender";
int txPin = TX;
int rxPin = RX;
int bytesSent = 0;
int bytesReceived = 0;
int totalBytesSent = 0;
int totalBytesReceived = 0;

void setup () {
	Serial.begin ( 115200 );
	pinMode ( SENDER_LED, OUTPUT );
	flashPin ( SENDER_LED );
	pinMode ( RECEIVER_LED, INPUT );
	flashPin ( RECEIVER_LED );
	showRoles ();

	while (txPin == 0 || rxPin == 0) {
		if (txPin == 0) txPin = assignTX ();
		if (rxPin == 0) rxPin = assignRX ();
	}

	transporter = new SoftwareSerial ( rxPin, txPin );
	transporter->begin ( 115200 );
	Serial.print ( F ( "Sending on pin " ) );
	Serial.println ( txPin );
	Serial.print ( F ( "Receiving on pin " ) );
	Serial.println ( rxPin );
	Serial.print ( name );
	Serial.println ( " READY" );
	Serial.println ( F ( "Enter text to be sent..." ) );
}

void loop () {
	showStats ();

	//	Check for incoming data
	while (Serial.available ()) {
		char dataToSend = Serial.read ();
		bytesSent += transporter->write ( dataToSend );
		Serial.print ( F ( "Sent: " ) );
		Serial.println ( dataToSend );
	}

	while (transporter->available ()) {
		char dataReceived = transporter->read ();
		bytesReceived++;
		if (ECHO) {
			transporter->write ( dataReceived );
			++bytesSent;
		}
		Serial.print ( F ( "Rcvd: " ) );
		Serial.println ( dataReceived );
	}
}

void showStats () {
	if (bytesSent > 0) {
		Serial.print ( F ( "Bytes sent: " ) );
		Serial.print ( bytesSent );
		Serial.print ( F ( "\ttotal: " ) );
		totalBytesSent += bytesSent;
		Serial.println ( totalBytesSent );
		bytesSent = 0;
	}
	if (bytesReceived > 0) {
		Serial.print ( F ( "Bytes received: " ) );
		Serial.print ( bytesReceived );
		Serial.print ( F ( "\ttotal: " ) );
		totalBytesReceived += bytesReceived;
		Serial.println ( totalBytesReceived );
		bytesReceived = 0;
	}
}

int assignTX () {
	Serial.println ( F ( "Enter TX Pin# (3-12):" ) );
	while (!Serial.available ()) {}
	txPin = Serial.parseInt ();
	if (txPin < 2 || txPin > 12) return 0;
	return txPin;
}

int assignRX () {
	Serial.println ( F ( "Enter RX Pin# (3-12):" ) );
	while (!Serial.available ()) {}
	rxPin = Serial.parseInt ();
	if (rxPin < 2 || rxPin > 12) return 0;
	return rxPin;
}

void showRoles () {
	if (ROLE_IS_RECEIVER) {
		turnOff ( SENDER_LED );
		turnOn ( RECEIVER_LED );
	} else {
		turnOn ( SENDER_LED );
		turnOff ( RECEIVER_LED );
	}
}


void turnOn ( int pin ) {
	digitalWrite ( pin, HIGH );
}

void turnOff ( int pin ) {
	digitalWrite ( pin, LOW );
}

void flashPin ( int pin ) {
	flashPin ( pin, false );

}

void flashPin ( int pin, bool leaveOn ) {
	flashPin ( pin, FLASH_TIMES, FLASH_DURATION );
	( leaveOn )?turnOn ( pin ):turnOff ( pin );
}

void flashPin ( int pin, int times, int duration ) {
	for (int i = 0; i < times; i++) {
		turnOn ( pin );
		delay ( duration );
		turnOff ( pin );
		delay ( duration );
	}
}