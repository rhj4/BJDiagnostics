#ifndef _TransporterTester_h
#include <SoftwareSerial.h>
#include "Transporter.h"
#define _TransporterTester_h
/*******************************************************************
*
*	Name:	Receover.ino
*	Author:	Bob Jones <bob@rhj4.com>
*	Date:	January 31, 2019
*
*	Test WiFi, Bluetooth amd Wired Serial communications protocols.
*
*	This sketch can be configured to test:
*
*		WiFi, Bluetooth or Wired Serial
*		Getter (receives commands) or Putter (sends commands)
*		Board type is Mega or (Uno / Nano)
*
*	For a Remote Controller, set the role to Putter.
*	For a Robot, Motor Controller, Dispatcher, Signaler, etc,
*	set the role to Getter.
*
*	There are four LEDs that provide diagnostic information:
*		GREEN (on pin 4) is ON if radio is OK
*		YELLOW (small LED on pin 7) flashes if role is transmitter
*		BLUE (on pin 6) is ON when sending data
*		GREEN (on pin 5) is ON when receiving data
*
*	If these diagnostics don't light up, then there is a problem.
*
********************************************************************/
#include "Config.h"
#if PROTOCOL_WIFI
#include "WiFi.h"
WiFi *handler;

#elif PROTOCOL_BLUETOOTH
#include "Bluetooth.h"
Bluetooth *handler;

#elif PROTOCOL_WIRED
#include "SerialWired.h"
SerialWired *handler;

#endif /* PROTOCOL_ */


void setup () {
	Serial.begin ( 115200 );
	while (!Serial.availableForWrite ()) {}
	if (LISTENING_LED > 0) {
		pinMode ( LISTENING_LED, OUTPUT );
	}

	if (SENDING_LED > 0) {
		pinMode ( SENDING_LED, OUTPUT );
	}

#if PROTOCOL_WIFI
	handler = new WiFi;
#elif PROTOCOL_BLUETOOTH
	handler = new Bluetooth;
#elif PROTOCOL_WIRED
	handler = new SerialWired;
#endif /* PROTOCOL_ */

	showLEDInfo ();
	handler->configure ();
	handler->showStatusLED ();
}

void showLEDInfo () {
	if (CONNECTED_ACTIVE_LED > 0) {
		Serial.print ( F ( "PIN " ));
		Serial.print ( CONNECTED_ACTIVE_LED );
		Serial.print ( F ( " IS CONNECTED LED: COLOR IS " ) );
		Serial.println ( CONNECTED_ACTIVE_COLOR );
		handler->flash ( CONNECTED_ACTIVE_LED );
	}

	if (LISTENING_LED > 0) {
		Serial.print ( F ( "PIN " ) );
		Serial.print ( LISTENING_LED );
		Serial.print ( F ( " IS LISTENING LED: COLOR IS " ) );
		Serial.println ( LISTENING_COLOR );
		handler->flash ( LISTENING_LED );
	}

	if (SENDING_LED > 0) {
		Serial.print ( F ( "PIN " ) );
		Serial.print ( SENDING_LED);
		Serial.print ( F ( " IS SENDING LED: COLOR IS " ) );
		Serial.println ( SENDING_COLOR );
		handler->flash ( SENDING_LED );
	}

}

void loop () {
	while (handler->dataAvailable ()) {
		Serial.print ( '.' );
		if (handler->getSentData() && handler->messageIsCompleted ()) {
			handler->showMessage ();
			handler->clearDataBuffer ();
		}
	}

	if (handler->getDataToSend ()) {
		Serial.print ( '+' );
		handler->sendDataBuffer ();
	}

	delay ( LOOP_DELAY );
	handler->showStatusLED ();
}
#endif /*  Receiver.ino */
