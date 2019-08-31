#ifndef _TransporterTester_h
#include "Transporter.h"
#define _TransporterTester_h
/*******************************************************************
*
*	Name:	Transmitter.ino
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

	//	Wait until hardware is ready
	while (!Serial.availableForWrite ()) {}

#if PROTOCOL_WIFI
	handler = new WiFi;
#elif PROTOCOL_BLUETOOTH
	handler = new Bluetooth(4,5);
#elif PROTOCOL_WIRED
	handler = new SerialWired;
#endif /* PROTOCOL_ */

	//	Configure LED Pins
	handler->configurePins ();

	//	Configure hardware
	handler->configure ();

	//	Show current hardware status 
	handler->showStatusLED ();
}

void loop () {
	//	Get any data sent across the wires
	while (handler->dataAvailable ()) {
		handler->turnOn ( LISTENING_LED, false );
		if (handler->getSentData () && handler->messageIsCompleted ()) {
			handler->showMessage ();
			handler->clearDataBuffer ();
		}
	}
	handler->turnOff ( LISTENING_LED, false );

	if (handler->getDataToSend ()) {
		handler->sending ( true );
		handler->sendDataBuffer ();
		handler->sending ( false );
	}

	delay ( LOOP_DELAY );
	handler->showStatusLED ();
}
#endif /*  Transmitter.ino */
