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

#if PROTOCOL_WIFI
	handler = new WiFi;
#elif PROTOCOL_BLUETOOTH
	handler = new Bluetooth;
#elif PROTOCOL_WIRED
	handler = new SerialWired;
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
	if (handler->messageIsCompleted ()) {
		handler->showMessage ();
	}
#if DOING_SOMETHING_LED > 0
	digitalWrite ( DOING_SOMETHING_LED, LOW );
#endif
#endif

	handler->showStatusLED ();

	delay ( LOOP_DELAY );

}

