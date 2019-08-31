#pragma once
class Bluetooth: public Transporter {
private:

protected:

public:
	//	Constructor
	Bluetooth () : Transporter () {
#if BOARD_TYPE_MEGA
		Serial.print ( F ( "MEGA " ) );
#else
		Serial.print ( F ( "UNO / NANO " ) );
#endif 
#if ROLE_IS_GETTER
		Serial.print ( F ( "GETTER " ) );
#else
		Serial.print ( F ( "PUTTER " ) );
#endif

		Serial.println ( F ( "Bluetooth Tester" ) );

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