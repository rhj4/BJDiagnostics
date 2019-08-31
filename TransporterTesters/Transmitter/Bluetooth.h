//	Bluetooth.h
#pragma once
#include "Config.h"
#if PROTOCOL_BLUETOOTH
#include "Transporter.h"

class Bluetooth: public Transporter {
private:

protected:

public:
	//	Constructor
	Bluetooth () : Transporter ( "SERIAL-BLUETOOTH" ) {
	}

	bool configure () {
		return false;
	}

	bool showStatusLED () {
		return false;
	}

	void clearDataBuffer () {
		for (unsigned int i = 0; i < sizeof ( dataBuffer ); i++) {
			dataBuffer [ i ] = 0x00;
		}
	}

	bool getSentData () {
		return false;
	}

	void showMessage () {
		if (strlen ( dataBuffer ) > 0) {
			Serial.print ( F ( "RECEIVED: " ) );
			Serial.println ( dataBuffer );
			return;
		}
		Serial.print ( F ( "NO DATA" ) );
	}

	bool dataAvailable () {
		return bluetooth->available ();
	}

	void showDataBuffer ( String label ) {
		if (!_tracing) return;

		Serial.print ( "dataBuffer: " );
		Serial.print ( label );
		Serial.print ( F ( " [" ) );
		Serial.print ( dataBuffer );
		Serial.println ( F ( "]" ) );
		//	dumpString(dataBuffer, sizeof(dataBuffer));
	}

	bool getDataToSend () {

		return false;
	}

	void sendDataBuffer () {

	}
};
#endif /* PROTOCOL_BLUETOOTH */