//	Serial Wired
#pragma once
#include "Config.h"
#if PROTOCOL_WIRED
#include <SoftwareSerial.h>
#include "Transporter.h"
class SerialWired: public Transporter {
private:

protected:

public:
	SoftwareSerial *transporter;

	int txPin = TX_PIN;
	int rxPin = RX_PIN;
	int bytesSent = 0;
	int bytesReceived = 0;
	int totalBytesSent = 0;
	int totalBytesReceived = 0;
	unsigned int index = 0;


	//	Constructor
	SerialWired (): Transporter ( "SERIAL-WIRED" ) {}

	bool configure () {
		while (txPin == 0 || rxPin == 0) {
			if (txPin == 0) txPin = assignTX ();
			if (rxPin == 0) rxPin = assignRX ();
		}

		transporter = new SoftwareSerial ( rxPin, txPin );
		transporter->begin ( 115200 );
		Serial.print ( F ( "SENDING ON PIN " ) );
		Serial.println ( txPin );
		Serial.print ( F ( "RECEIVING ON PIN " ) );
		Serial.println ( rxPin );

		Serial.print ( name );
		Serial.println ( " READY" );
		Serial.println ( F ( "Enter text to be sent..." ) );

		showStatusLEDs ();

		return true;
	}

	bool getDataToSend () {
		if (!Serial.available ()) {
			return false;
		}

		clearDataBuffer ();
		byte data;
		index = 0;
		while (Serial.available ()) {
			data = Serial.read ();
			if (data < 0x20) continue;

			if (index < ( DATA_BUFFER_SIZE - 1 )) {
				dataBuffer [ index++ ] = data;
				dataBuffer [ index ] = 0x00;
			} else {
				Serial.println ( F ( "\n*** TOO MUCH DATA ***" ) );
				dumpString ( dataBuffer, DATA_BUFFER_SIZE );
				while (Serial.available ()) { 
					Serial.read (); 
				};
				clearDataBuffer ();
				return false;
			}
			delay ( 1 );
		}

		if (strlen ( dataBuffer ) == 0) return false;

		if (_tracing) {
			Serial.print ( F ( "\nSending " ) );
			Serial.print ( strlen ( dataBuffer ) );
			Serial.print ( F ( " bytes in " ) );
			showDataBuffer ( "" );
		}

		delay ( 10 );
		return true;
	}

	bool getSentData () {
		index = 0;
		while (transporter->available ()) {
			char data = transporter->read ();
			dataBuffer [ index++ ] = data;
			dataBuffer [ index ] = 0x00;
		}

		//if (index > 0) {
		//	Serial.println ();
		//	Serial.println ( dataBuffer );
		//}

		return ( index > 0 );
	}

	//	Check for data in Serial buffer to be processed
	bool dataAvailable () {
		return transporter->available ();
	}

	void sendDataBuffer () {
		// Ensure we have stopped listening (even if we're not) or we won't be able to transmit

		sending ( true );
		transporter->write ( dataBuffer );
		clearDataBuffer ();
		sending ( false );
	}

	void showMessage () {
		if (strlen ( dataBuffer ) > 0) {
			Serial.print ( F ( "RECEIVED: " ) );
			Serial.println ( dataBuffer );
			return;
		}
		Serial.print ( F ( "NO DATA" ) );
	}

	bool showStatusLED () {
		return false;
	}

	bool messageIsCompleted () {
		return (strlen(dataBuffer) > 0);
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
};
#endif /* PROTOCOL_WIRED */