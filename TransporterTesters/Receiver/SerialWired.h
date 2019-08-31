#pragma once
#include "Config.h"
#include "Transporter.h"
#include <SoftwareSerial.h>

class SerialWired: public Transporter {
private:

protected:
	int txPin = TX_PIN;
	int rxPin = RX_PIN;
	SoftwareSerial *transporter;
	uint16_t bytesSent = 0;

public:
	//	Constructor
	SerialWired (): Transporter () {}

	bool configure () {
		while (txPin == 0 || rxPin == 0) {
			if (txPin == 0) txPin = assignPin ( ( char* )"TX" );
			if (rxPin == 0) rxPin = assignPin ( ( char* )"RX" );
		}

		transporter = new SoftwareSerial ( rxPin, txPin );
		transporter->begin ( 115200 );
		Serial.print ( F ( "RECEIVING ON PIN " ) );
		Serial.println ( rxPin );
		Serial.print ( F ( "SENDING ON PIN " ) );
		Serial.println ( txPin );

		return true;
	}

	int assignPin (char* pinName) {
		Serial.print ( F ( "Enter "));
		Serial.print(pinName);
		Serial.println(F(" Pin# (3-12):" ) );
		while (!Serial.available ()) {}
		int pin = Serial.parseInt ();
		if (pin < 3 || pin > 12) return 0;
		return pin;
	}

	bool getDataToSend () {
		if (!Serial.available ()) return false;

		int dataBufferIndex = strlen ( dataBuffer );
		while (Serial.available ()) {
			uint8_t dataToSend = ( uint8_t ) Serial.read ();
			dataBuffer [ dataBufferIndex++ ] = dataToSend;
			dataBuffer [ dataBufferIndex ] = 0x00;
		}

		return (strlen(dataBuffer) > 0);
	}

	bool getSentData () {
		return false;
	}

	bool dataAvailable () {
		return ( strlen ( dataBuffer ) > 0 );
	}

	void sendDataBuffer () {
		transporter->write ( dataBuffer );
	}

	void showMessage () {
		Serial.println ( dataBuffer );
	}

	bool showStatusLED () {
		return false;
	}

	bool messageIsCompleted () {
		return true;
	}
};
