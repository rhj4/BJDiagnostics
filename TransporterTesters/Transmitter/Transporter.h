// Transporter.h
#pragma once
#include "arduino.h"
#include "Config.h"

class Transporter {
protected:
	char *name = ( ROLE_IS_GETTER ) ? ( char* )"RECEIVER" : ( char* )"SENDER";

	bool _tracing = TRACING;
	char message [ MESSAGE_SIZE ] = {};
	char dataBuffer [ DATA_BUFFER_SIZE ] = {};

public:
	Transporter ( char* protocol ) {
#if BOARD_TYPE_MEGA
		Serial.print ( F ( "MEGA " ) );
#else
		Serial.print ( F ( "UNO / NANO " ) );
#endif 
		Serial.print ( protocol );

#if ROLE_IS_GETTER
		Serial.print ( F ( " GETTER " ) );
#else
		Serial.print ( F ( " PUTTER " ) );
#endif

		Serial.println ();
	}

	virtual bool configure () = 0;
	virtual bool getDataToSend () = 0;
	virtual void sendDataBuffer () = 0;
	virtual bool messageIsCompleted () = 0;
	virtual bool showStatusLED () = 0;

	void clearDataBuffer () {
		for (unsigned int i = 0; i < DATA_BUFFER_SIZE; i++) {
			dataBuffer [ i ] = 0x00;
		}
	}

	void showDataBuffer ( String label ) {
		if (!_tracing) return;

		Serial.print ( "dataBuffer: " );
		Serial.print ( label );
		Serial.print ( F ( " [" ) );
		Serial.print ( dataBuffer );
		Serial.println ( F ( "]" ) );
	}

	void clearMessage () {
		for (unsigned int i = 0; i < sizeof ( message ); i++) {
			message [ i ] = 0x00;
		}
	}

	void dumpString ( String buffer, unsigned int size ) {
		if (!_tracing) return;

		unsigned int col0 = 0;
		char letters [ COLS + 1 ] = {};
		for (unsigned int i = 1; i <= size; i++) {
			printHexByte ( buffer [ i - 1 ] );
			if (( i >= COLS && i % COLS == 0 ) || ( i > size - 1 )) {
				Serial.print ( '\t' );
				unsigned int k = 0;
				for (unsigned int j = col0; j < col0 + COLS; j++) {
					letters [ k++ ] = ( buffer [ j ] < 0x20 ) ? '.' : ( char ) buffer [ j ];
				}
				letters [ COLS ] = 0;
				col0 = i + 1;
				Serial.println ( letters );
			} else {
				Serial.print ( ' ' );
			}
		}
		Serial.println ();
	}

	void printHexByte ( byte b ) {
		if (b < 0X10) Serial.print ( '0' );
		Serial.print ( b, HEX );
		Serial.print ( ' ' );
	}

	void turnOn ( int pin, bool trace ) {
		digitalWrite ( pin, HIGH );
		if (trace && _tracing) {
			Serial.print ( F ( "TURNING ON PIN " ) );
			Serial.println ( pin );
		}
	}

	void turnOn ( int pin ) {
		turnOn ( pin, true );
	}

	void turnOff ( int pin, bool trace ) {
		digitalWrite ( pin, LOW );
		if (trace && _tracing) {
			Serial.print ( F ( "TURNING OFF PIN " ) );
			Serial.println ( pin );
		}
	}

	void turnOff ( int pin ) {
		turnOff ( pin, true );
	}

	void flash ( int pin, unsigned int count, unsigned long duration ) {
		for (unsigned int i = 0; i < count; i++) {
			turnOn ( pin, false );
			delay ( duration );
			turnOff ( pin, false );
			delay ( duration );
		}
	}

	void flash ( int pin ) {
		flash ( pin, FLASH_COUNT, FLASH_DURATION );
	}

	void sending ( bool value ) {
		if (SENDING_LED > 0) {
			if (value) {
				turnOn ( SENDING_LED, false );
			} else {
				turnOff ( SENDING_LED, false );
			}

			delay ( 10 );
		}
	}

	void showStatusLEDs () {
		if (ROLE_IS_PUTTER) {
			turnOn ( TRANSMITTER_LED, false );
		} else {
			turnOff ( TRANSMITTER_LED, false );
		}

		turnOn ( CONNECTED_ACTIVE_LED, false );
	}

	void configurePins () {
		if (CONNECTED_ACTIVE_LED > 0) {
			pinMode ( CONNECTED_ACTIVE_LED, OUTPUT );
			Serial.print ( CONNECTED_ACTIVE_COLOR );
			Serial.print ( F ( " LED ON PIN " ) );
			Serial.print ( CONNECTED_ACTIVE_LED );
			Serial.println ( F ( " ON MEANS \"CONNECTED\"" ) );
			flash ( CONNECTED_ACTIVE_LED );
		}

		if (LISTENING_LED > 0) {
			pinMode ( LISTENING_LED, OUTPUT );
			Serial.print ( LISTENING_COLOR );
			Serial.print ( F ( " LED ON PIN " ) );
			Serial.print ( LISTENING_LED );
			Serial.println ( F ( " ON MEANS \"LISTENING\"" ) );
			flash ( LISTENING_LED );
		}

		if (SENDING_LED > 0) {
			pinMode ( SENDING_LED, OUTPUT );
			Serial.print ( SENDING_COLOR );
			Serial.print ( F ( " LED ON PIN " ) );
			Serial.print ( SENDING_LED );
			Serial.println ( F ( " ON MEANS \"SENDING\"" ) );
			flash ( SENDING_LED );
		}

		if (ROLE_IS_PUTTER && TRANSMITTER_LED > 0) {
			pinMode ( TRANSMITTER_LED, OUTPUT );
			Serial.print ( TRANSMITTER_COLOR );
			Serial.print ( F ( " LED ON PIN " ) );
			Serial.print ( TRANSMITTER_LED );
			Serial.println ( F ( " ON MEANS \"TRANSMITTER\"" ) );
			flash ( TRANSMITTER_LED );
		}
	}
};
