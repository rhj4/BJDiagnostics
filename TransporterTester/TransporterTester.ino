#ifndef _TransporterTester_h
#include "Transporter.h"
#define _TransporterTester_h
/*******************************************************************
*
*	Name:	TransporterTester.h
*	Author:	Bob Jones <bob@rhj4.com>
*	Date:	January 31, 2019
*
*	Test WiFi and Bluetooth communications protocols.
*
*	This sketch can be configured to test:
*
*		WiFi or Bluetooth
*		Getter (receives commands) or Putter (sends commands)
*		Board type is Mega or (Uno / Nano)
*
*	For a Remote Controller, set the role to Putter.
*	For a Robot, Motor Controller, Dispatcher, Signaler, etc,
*	set the role to Getter.
*
********************************************************************/
#include "Config.h"
#if PROTOCOL_WIFI
#include "WiFi.h"
WiFi *handler;

#elif PROTOCOL_BLUETOOTH
#include "Bluetooth.h"
Bluetooth *handler;
#endif /* PROTOCOL_ */


void setup () {
	Serial.begin ( 115200 );
	while (!Serial.availableForWrite ()) {}

#if PROTOCOL_WIFI
	handler = new WiFi;
#elif PROTOCOL_BLUETOOTH
	handler = new Bluetooth;
#endif /* PROTOCOL_ */

	handler->configure ();

}

void loop () {
#if ROLE_IS_PUTTER
	if (handler->getDataToSend ()) {

#if DOING_SOMETHING_LED > 0
		digitalWrite ( DOING_SOMETHING_LED, HIGH );
#endif
		handler->sendDataBuffer ();

#if DOING_SOMETHING_LED > 0
		digitalWrite ( DOING_SOMETHING_LED, LOW );
#endif

		handler->sendDataBuffer ();
	} else {
		Serial.flush ();
	}

	handler->clearDataBuffer ();
#else
	// Is there any data for us to get?
	if (!handler->dataAvailable ()) {
		delay ( 100 );
		return;
	}
#if DOING_SOMETHING_LED > 0
	digitalWrite ( DOING_SOMETHING_LED, HIGH );
#endif
	handler->getSentData ();
#if DOING_SOMETHING_LED > 0
	digitalWrite ( DOING_SOMETHING_LED, LOW );
#endif
#endif

	handler->showStatusLED ();

	delay ( LOOP_DELAY );

}

#endif /* _TransporterTester_h */
