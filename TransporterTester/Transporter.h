// Transporter.h

#ifndef _TRANSPORTER_h
#define _TRANSPORTER_h
#include "arduino.h"
#include "Config.h"

class Transporter
{
protected:
	 bool _tracing = true;
	 char message [ MESSAGE_SIZE ] = {};
	 char dataBuffer [ DATA_BUFFER_SIZE ] = {};

public:
	 Transporter () {
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

		 Serial.println ( F ( "WiFi Tester" ) );
	 }

	 virtual bool configure () = 0;
	 virtual bool getDataToSend () = 0;
	 virtual void sendDataBuffer () = 0;

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


};


#endif

